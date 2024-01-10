#pragma once

#include <unordered_set>
#include <list>
#include "implot.h"
#include "MBIMGUI.h"
#include "MBIRealtimePlotChart.h"

/**
 * @brief Display optimization : compute display data area size.
 * ----------------------
 * Because ImPlot process all points, even ones not currently
 * displayed on the graph (because of zoom, axis offset etc.), we
 * can experience huge cpu load with large amount of points.
 *
 * To avoid this, we compute the exact set of points needed for the
 * currently displayed graph portion.
 *
 * Because of float rounding, and because it is hard to handle every
 * case specifically, we take a margin on size and offset during calculation
 * to ensure that a maximum of data are displayed on the graph.
 *
 */
inline void MBIRealtimePlotChart::ComputeDataWindow(DataRender &dataRenderInfos, size_t &dataSize, int32_t &dataOffset)
{
    if (dataRenderInfos.descriptor.bHidden == false)
    {
        /* Get data range available */
        const double dataBegin = dataRenderInfos.data->first().m_time;
        const double dataEnd = dataRenderInfos.data->last().m_time;

        /* Depending on the case */
        if (dataBegin > m_xAxisRange.Max || dataEnd < m_xAxisRange.Min)
        {
            /* CASE 1 : All data are outside the graph window --> Don't draw anything */
            dataOffset = 0;
            dataSize = 0;
        }
        else if (dataBegin >= m_xAxisRange.Min && dataEnd <= m_xAxisRange.Max)
        {
            /* CASE 2 : All data are within the graph window, draw everything */
            dataOffset = 0;
            dataSize = dataRenderInfos.data->size();
        }
        else if (dataBegin >= m_xAxisRange.Min)
        {
            /* CASE 3: First data point is within the graph, last data point is outer right */
            /* Draw from start */
            dataOffset = 0;
            /* Compute remaining size */
            dataSize = ((uint32_t)((m_xAxisRange.Max - dataBegin) * 1000.0)) / dataRenderInfos.dataPeriodMs;
            /* Add margin */
            dataSize++;
        }
        else if (dataEnd <= m_xAxisRange.Max)
        {
            /* CASE 4 : First data point is outer left of the graph and last point is within the graph */
            /* Compute first point offset */
            dataOffset = ((uint32_t)((m_xAxisRange.Min - dataBegin) * 1000.0)) / dataRenderInfos.dataPeriodMs;
            /* Draw till last point */
            dataSize = dataRenderInfos.data->size() - dataOffset;
            /* Add margins */
            if (dataRenderInfos.dataPeriodMs > 1)
            {
                dataOffset -= 3;
                dataSize += 3;
            }
            else
            {
                dataOffset--;
                dataSize++;
            }
        }
        else
        {
            /* CASE 5 : Both points are out, we draw a slice of data */
            /* Compute first point offset */
            dataOffset = ((uint32_t)((m_xAxisRange.Min - dataBegin) * 1000.0)) / dataRenderInfos.dataPeriodMs;
            /* Compute remaining size */
            dataSize = ((uint32_t)((m_xAxisRange.Max - m_xAxisRange.Min) * 1000.0)) / dataRenderInfos.dataPeriodMs;
            /* Add margins */
            if (dataRenderInfos.dataPeriodMs > 1)
            {
                dataOffset -= 3;
            }
            else
            {
                dataOffset--;
            }
            dataSize += 3;
        }

        /* Security checks : to ensure graph window is fully filled with data,
         we took margins for computation (due to float rounding).
         Check that we didn't overcome limits. */

        /* Rolled back from 0 ? */
        if (dataOffset < 0)
        {
            dataOffset = 0;
        }
        /* Exceed total data size ? */
        if (dataOffset + dataSize > dataRenderInfos.data->size())
        {
            dataSize = dataRenderInfos.data->size() - dataOffset;
        }
        /* ************************************************************* */
    }
}

void MBIRealtimePlotChart::Display(double currentTimeS)
{
    static bool nodata = true;
    static bool logwarning = true;

    UnitId unitList[3] = {DataDescriptor::INVALID_UNIT, DataDescriptor::INVALID_UNIT, DataDescriptor::INVALID_UNIT};
    int unitCounter = 0;
    bool bNewUnit = true;
    bool bAxesMoved = false;
    ImAxis axis = ImAxis_COUNT;

    /* Set legend outside the graph, at the top */
    ImPlot::SetupLegend(ImPlotLocation_North, ImPlotLegendFlags_Outside);

    /* Set x-axis */
    ImPlot::SetupAxis(ImAxis_X1, "Time", ImPlotAxisFlags_NoMenus);
    ImPlot::SetupAxisLimits(ImAxis_X1, (currentTimeS - m_history), currentTimeS, (m_pause) ? ImGuiCond_Once : ImGuiCond_Always);

    /* Link axis if requested */
    if (m_pause)
    {
        ImPlot::SetupAxisLinks(ImAxis_X1, &m_xAxisRange.Min, &m_xAxisRange.Max);
    }

    /* If data available */
    /* Setup axes */
    for (auto it = m_vargaph.begin(); it != m_vargaph.end(); it++)
    {
        const DataDescriptor &dataDescriptor = GetDataDescriptor(*it);

        bNewUnit = true;
        /* Look for an existing variable with the same unit */
        for (int i = 0; i < unitCounter; i++)
        {
            if (dataDescriptor.unit.first == unitList[i])
            {
                axis = ImAxis_Y1 + i;
                bNewUnit = false;
                break;
            }
        }
        /* If new Unit added, setup a new axe */
        if (bNewUnit)
        {
            /* ImPlot can't manage more than 3 y-axes */
            if (unitCounter < 3)
            {
                axis = ImAxis_Y1 + unitCounter;
                unitList[unitCounter++] = dataDescriptor.unit.first;
                ImPlot::SetupAxis(axis, dataDescriptor.unit.second.c_str());
            }
            else
            {
                axis = ImAxis_Y1;
                if (logwarning)
                {
                    MBIMGUI::GetLogger().Log(MBIMGUI::LOG_LEVEL_WARNING, "Maximum three different axes on the same graph supported");
                    logwarning = false;
                }
            }
        }
        /* Set y-axis for data */
        GetDataRenderInfos(*it).descriptor.axis = axis;
    }

    /* Set x-axis */
    DisplayMarkers(UNIT_TIME_X_AXIS);

    /* Draw all variables */
    for (auto it = m_vargaph.begin(); it != m_vargaph.end(); it++)
    {
        size_t dataSize = 0;
        int32_t dataOffset = 0;
        const VarId varId = *it;
        /* Get data descriptor */
        DataRender &dataRenderInfos = GetDataRenderInfos(varId);
        if (dataRenderInfos.data->empty() == false)
        {
            /* If data is shown */

            /* If data has just been moved, force visibility */
            if (dataRenderInfos.descriptor.bMoved)
            {
                ImPlot::HideNextItem(false, ImPlotCond_Always);
                dataRenderInfos.descriptor.bMoved = false;
            }
            /* Set y-axis and plot line color */
            ImPlot::SetAxis(dataRenderInfos.descriptor.axis);
            ImPlot::SetNextLineStyle(dataRenderInfos.descriptor.color);

#if 1
            /* Try to optimize large amount of data */
            ComputeDataWindow(dataRenderInfos, dataSize, dataOffset);
#else
            /* Draw full data range */
            dataSize = dataRenderInfos.data->size();
            dataOffset = 0;
#endif

            dataRenderInfos.dataOffset = dataOffset;
            /* Draw line even if data are hidden because PlotLine draws legend */
            if (dataSize > m_downSamplingSize && m_activDownSampling == true && m_pause == false)
            {
                /* Down sample data only if needed (avoid parsing whole data set each frame) */
                if (m_dsUpdate == true)
                {
                    dataRenderInfos.DownSampleLTTB(dataOffset, (int)dataSize, (int)m_downSamplingSize);
                }
                ImPlot::PlotLine(dataRenderInfos.descriptor.name.c_str(), &dataRenderInfos.dsData[0].m_time, &dataRenderInfos.dsData[0].m_data, dataRenderInfos.dsData.Size, ImPlotLineFlags_None, 0, 2 * sizeof(double));
                m_downSampled = true;
            }
            else
            {
                const DataContainer &datapoints = (*dataRenderInfos.data);
                // WARNING: PlotLine using stride requires contiguous data storage type. This is not the case of the MBICircularBuffer, so we have to use DataGetter
                //ImPlot::PlotLine(dataRenderInfos.descriptor.name.c_str(), &datapoints[dataOffset].m_time, &datapoints[dataOffset].m_data, dataSize, ImPlotLineFlags_None, 0, 2 * sizeof(double));
                ImPlot::PlotLineG(dataRenderInfos.descriptor.name.c_str(), DataGetter, (void *)&dataRenderInfos, (int)dataSize);

                if (dataRenderInfos.descriptor.bHidden == false)
                {
                    m_downSampled = false;
                }
            }

            /* Draw Annotation */
            if (dataRenderInfos.descriptor.bShowAnnotations && dataRenderInfos.annotation != nullptr)
            {
                auto it = dataRenderInfos.annotation->cbegin();
                while (it != dataRenderInfos.annotation->cend() && it->m_x < m_xAxisRange.Max)
                {
                    if (it->m_x >= m_xAxisRange.Min)
                    {
                        ImPlot::Annotation(it->m_x, it->m_y, ImVec4(255, 255, 255, 255), ImVec2(5, -5), false, it->GetLabel());
                    }
                    it++;
                }
            }
        }

        /* Drag and Drop */
        if (ImPlot::BeginDragDropSourceItem(dataRenderInfos.descriptor.name.c_str()))
        {
            ImGui::SetDragDropPayload(DND_LABEL_FROM_GRAPH, &varId, sizeof(varId));
            ImPlot::ItemIcon(ImPlot::GetLastItemColor());
            ImPlot::EndDragDropSource();
        }

        /* Catch legend click to get data visibility */
        if (ImPlot::IsLegendEntryHovered(dataRenderInfos.descriptor.name.c_str()) && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            dataRenderInfos.descriptor.bHidden = !dataRenderInfos.descriptor.bHidden;
        }

        /* Update y-axis range for next frame */
        const ImAxis axisOffset = GetYAxisOffset(dataRenderInfos.descriptor.unit.first);
        const ImPlotRect plotLimits = ImPlot::GetPlotLimits(IMPLOT_AUTO, dataRenderInfos.descriptor.axis);

        /* If down sampling is active */
        if (m_activDownSampling)
        {
            /* Detect a plot limits modification (zoom in/out, scale modifications...) */
            if ((m_yAxesRange[axisOffset].Max != plotLimits.Y.Max) || (m_yAxesRange[axisOffset].Min != plotLimits.Y.Min))
            {
                m_yAxesRange[axisOffset] = plotLimits.Y;
                /* Notify modification for downsampling */
                bAxesMoved = true;
            }
        }
        else
        {
            m_xAxisRange = plotLimits.X;
            m_yAxesRange[axisOffset] = plotLimits.Y;
        }

        DisplayMarkers(dataRenderInfos.descriptor.unit.first);
    }

    /* If no var, still display markers for no unit */
    if (m_vargaph.empty())
    {
        DisplayMarkers(UNIT_NONE);
    }

    /* Update axis range for next frame */
    const ImPlotRect plotLimits = ImPlot::GetPlotLimits();
    /* If down sampling is active */
    if (m_activDownSampling)
    {
        /* Detect a plot limits modification (zoom in/out, scale modifications...) */
        if ((m_xAxisRange.Max != plotLimits.X.Max) || (m_xAxisRange.Min != plotLimits.X.Min))
        {
            m_xAxisRange = plotLimits.X;
            /* Recompute down sampled data next frame */
            bAxesMoved = true;
        }
        m_dsUpdate = bAxesMoved;
    }
    else
    {
        m_xAxisRange = plotLimits.X;
    }

    /* Make plot and legend a drag and drop target */
    if (ImPlot::BeginDragDropTargetPlot() || ImPlot::BeginDragDropTargetLegend())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(m_dndType.c_str()))
        {
            if (m_callback != nullptr)
            {
                m_callback(m_callbackArg, payload);
            }
        }
        ImPlot::EndDragDropTarget();
    }
}

void MBIRealtimePlotChart::Pause(bool pause) noexcept
{
    m_pause = pause;
}

void MBIRealtimePlotChart::SetHistory(float history) noexcept
{
    m_history = history;
}

void MBIRealtimePlotChart::AddDataAnnotations(const VarId &dataId, const MBISyncCircularBuffer<DataAnnotation> *const dataAnnotationPtr)
{
    GetDataRenderInfos(dataId).annotation = dataAnnotationPtr;
}

MBIRealtimePlotChart::VarId MBIRealtimePlotChart::CreateVariable(const DataContainer *const dataPtr, uint32_t period)
{
    DataRender *dataRender = nullptr;
    const VarId id = MakeUUID();

    /* Must alocate a new data renderer infos ? */
    if (m_varData.find(id) == m_varData.end())
    {
        dataRender = new DataRender(dataPtr);
        dataRender->dataPeriodMs = period;
        m_varData[id] = dataRender;
    }

    AddVariable(id);

    return id;
}

void MBIRealtimePlotChart::SetDataDescriptorHandle(const VarId &dataId, DataDescriptorHandle dataRender)
{
    if (m_varData.find(dataId) != m_varData.end())
    {
        delete m_varData.find(dataId)->second;
    }
    m_varData[dataId] = new DataRender((const DataRender *const)dataRender);
}

MBIRealtimePlotChart::DataDescriptorHandle MBIRealtimePlotChart::GetDataDescriptorHandle(const VarId &dataId) const
{
    /* Return data */
    auto it = m_varData.find(dataId);
    if (it != m_varData.end())
    {
        return (it->second);
    }
    else
    {
        /* Return invalid data */
        return (m_varData.find(0xFFFFFFFF)->second);
    }
}

const DataDescriptor &MBIRealtimePlotChart::GetDataDescriptor(const VarId &dataId) const
{
    /* Return data */
    auto it = m_varData.find(dataId);
    if (it != m_varData.end())
    {
        return it->second->descriptor;
    }
    else
    {
        /* Return invalid data */
        return m_varData.find(0xFFFFFFFF)->second->descriptor;
    }
}

DataDescriptor &MBIRealtimePlotChart::GetDataDescriptor(const VarId &dataId)
{
    /* Force calling const impl by casting this as const */
    return const_cast<DataDescriptor &>(const_cast<const MBIRealtimePlotChart *>(this)->GetDataDescriptor(dataId));
}

const MBIRealtimePlotChart::DataRender &MBIRealtimePlotChart::GetDataRenderInfos(const VarId &dataId) const
{
    /* Return data */
    auto it = m_varData.find(dataId);
    if (it != m_varData.end())
    {
        return *(it->second);
    }
    else
    {
        /* Return invalid data */
        return *(m_varData.find(0xFFFFFFFF)->second);
    }
}

MBIRealtimePlotChart::DataRender &MBIRealtimePlotChart::GetDataRenderInfos(const VarId &dataId)
{
    /* Force calling const impl by casting this as const */
    return const_cast<DataRender &>(const_cast<const MBIRealtimePlotChart *>(this)->GetDataRenderInfos(dataId));
}

MBIRealtimePlotChart::MBIRealtimePlotChart() : m_pause(true),
                                               m_history(10.0)
{
    /* Create default invalid data descriptor */
    DataRender *dataRender = new DataRender(nullptr, false);
    dataRender->dataPeriodMs = 0;
    m_varData[0xFFFFFFFF] = dataRender;
}

MBIRealtimePlotChart::~MBIRealtimePlotChart()
{
    for (auto servData : m_varData)
        delete servData.second;
}
