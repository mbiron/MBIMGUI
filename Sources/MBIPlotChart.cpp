#pragma once

#include <unordered_set>
#include <list>
#include "implot.h"
#include "MBIMGUI.h"
#include "MBIPlotChart.h"

void MBIPlotChart::Display(double currentTimeS)
{
    static bool nodata = true;
    static bool logwarning = true;

    UnitId unitList[3] = {INVALID_UNIT, INVALID_UNIT, INVALID_UNIT};
    int unitCounter = 0;
    bool bNewUnit = true;
    bool bAxesMoved = false;
    ImAxis axis = ImAxis_COUNT;

    /* Draw curves */

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
        const DataRenderInfos &dataInfos = GetDataDescriptor(*it);
        /* If variable is displayed on the graph */
        if (dataInfos.bHidden == false)
        {
            bNewUnit = true;
            /* Look for an existing variable with the same unit */
            for (int i = 0; i < unitCounter; i++)
            {
                if (dataInfos.unit.first == unitList[i])
                {
                    axis = ImAxis_Y1 + i;
                    bNewUnit = false;
                    break;
                }
            }

            if (bNewUnit)
            {
                /* ImPlot can't manage more than 3 y-axes */
                if (unitCounter < 3)
                {
                    axis = ImAxis_Y1 + unitCounter;
                    unitList[unitCounter++] = dataInfos.unit.first;
                    ImPlot::SetupAxis(axis, dataInfos.unit.second.c_str());
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
            GetDataDescriptor(*it).axis = axis;
        }
    }

    /* Set x-axis */
    ImPlot::SetAxis(ImAxis_X1);
    DisplayMarkers(UNIT_TIME_X_AXIS);

    /* Draw all variables */
    for (auto it = m_vargaph.begin(); it != m_vargaph.end(); it++)
    {
        size_t dataSize = 0;
        int32_t dataOffset = 0;
        const VarId type = *it;
        /* Get data descriptor */
        DataRenderInfos &dataRenderInfos = GetDataDescriptor(type);

        if (dataRenderInfos.data->empty() == false)
        {
            /* If data is shown */

            /* If data has just been moved, force visibility */
            if (dataRenderInfos.bMoved)
            {
                ImPlot::HideNextItem(false, ImPlotCond_Always);
                dataRenderInfos.bMoved = false;
            }
            /* Set y-axis and plot line color */
            ImPlot::SetAxis(dataRenderInfos.axis);
            ImPlot::SetNextLineStyle(dataRenderInfos.color);

            if (dataRenderInfos.bHidden == false)
            {
                /**********************************************************
                 * Display optimization : compute display data area size.
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
                 *********************************************************/

                /* Get data range available */
                const float dataBegin = dataRenderInfos.data->first().m_time;
                const float dataEnd = dataRenderInfos.data->last().m_time;

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
            dataRenderInfos.dataOffset = dataOffset;
            /* Draw line even if data are hidden because PlotLine draws legend */
            if (dataSize > m_downSamplingSize && m_pause == true && m_activDownSampling == true)
            {
                /* Down sample data only if needed (avoid parsing whole data set each frame) */
                if (m_dsUpdate == true)
                {
                    dataRenderInfos.DownSampleLTTB(dataOffset, (int)dataSize, (int)m_downSamplingSize);
                }
                ImPlot::PlotLineG(dataRenderInfos.name.c_str(), DataRenderInfos::PrtDsDataGetter, (void *)&dataRenderInfos, (int)m_downSamplingSize);
                m_downSampled = true;
            }
            else
            {
                // TODO : Test stride ?
                ImPlot::PlotLineG(dataRenderInfos.name.c_str(), DataRenderInfos::PrtDataGetter, (void *)&dataRenderInfos, (int)dataSize);
                if (dataRenderInfos.bHidden == false)
                {
                    m_downSampled = false;
                }
            }

            /* Draw Annotation */
            if (dataRenderInfos.bShowAnnotations && dataRenderInfos.annotation != nullptr)
            {
                auto it = dataRenderInfos.annotation->cbegin();
                while (it != dataRenderInfos.annotation->cend() && it->m_x < m_xAxisRange.Max)
                {
                    if (it->m_x >= m_xAxisRange.Min)
                    {
                        ImPlot::Annotation(it->m_x, it->m_y, ImVec4(255, 255, 255, 255), ImVec2(5, -5), false, it->getLabel());
                    }
                    it++;
                }
            }
        }

        /* Drag and Drop */
        if (ImPlot::BeginDragDropSourceItem(dataRenderInfos.name.c_str()))
        {
            ImGui::SetDragDropPayload("ParamFromGraph", &type, sizeof(type));
            ImPlot::ItemIcon(ImPlot::GetLastItemColor());
            ImPlot::EndDragDropSource();
        }

        /* Catch legend click to get data visibility */
        if (ImPlot::IsLegendEntryHovered(dataRenderInfos.name.c_str()) && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            dataRenderInfos.bHidden = !dataRenderInfos.bHidden;
        }

        /* Update y-axis range for next frame */
        const ImAxis axisOffset = GetYAxisOffset(dataRenderInfos.unit.first);
        const ImPlotRect plotLimits = ImPlot::GetPlotLimits(IMPLOT_AUTO, dataRenderInfos.axis);

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

        DisplayMarkers(dataRenderInfos.unit.first);
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
}

bool MBIPlotChart::RemoveVariable(const VarId &dataId)
{
    return (m_vargaph.erase(dataId) == 1);
}

void MBIPlotChart::SetVarName(const VarId &dataId, const std::string_view name)
{
    if (IsVariableOnGraph(dataId))
    {
        GetDataDescriptor(dataId).name = name;
    }
}

void MBIPlotChart::SetVarColor(const VarId &dataId, const ImVec4 &color)
{
    if (IsVariableOnGraph(dataId))
    {
        GetDataDescriptor(dataId).color = color;
    }
}

void MBIPlotChart::SetVarUnit(const VarId &dataId, const DataUnit &unit)
{
    if (IsVariableOnGraph(dataId))
    {
        GetDataDescriptor(dataId).unit = unit;
    }
}

void MBIPlotChart::ToggleVarAnnotation(const VarId &dataId, bool activ)
{
    if (IsVariableOnGraph(dataId))
    {
        GetDataDescriptor(dataId).bShowAnnotations = activ;
    }
}

ImAxis MBIPlotChart::GetYAxisOffset(const UnitId &eUnit) const
{
    for (const VarId &data : m_vargaph)
    {
        if (GetDataDescriptor(data).unit.first == eUnit)
        {
            return GetDataDescriptor(data).axis - ImAxis_Y1;
        }
    }
    return ImAxis_COUNT;
}

const ImPlotRange &MBIPlotChart::GetYAxisRange(const UnitId &eUnit) const
{
    ImAxis axisOffset = GetYAxisOffset(eUnit);
    if (axisOffset == ImAxis_COUNT)
    {
        axisOffset = 0;
    }
    return m_yAxesRange[axisOffset];
}

const ImPlotRange &MBIPlotChart::GetXAxisRange() const
{
    return m_xAxisRange;
}

void MBIPlotChart::SetXAxisRange(const ImPlotRange &range)
{
    m_xAxisRange = range;
}

const std::list<MBIPlotChart::Marker> &MBIPlotChart::GetMarkersList() const
{
    return m_markers;
}

std::list<MBIPlotChart::Marker> &MBIPlotChart::GetMarkersList()
{
    return m_markers;
}

void MBIPlotChart::AddMarker(const Marker &marker)
{
    m_markers.push_back(marker);
}

void MBIPlotChart::Pause(bool pause)
{
    m_pause = pause;
}

void MBIPlotChart::SetHistory(float history)
{
    m_history = history;
}

void MBIPlotChart::SetDownSampling(bool bActiv, size_t size)
{
    m_activDownSampling = bActiv;
    m_downSamplingSize = size;
}

void MBIPlotChart::AddDataAnnotations(const VarId &dataId, const MBISyncCircularBuffer<DataAnnotation> *const dataAnnotationPtr)
{
    GetDataDescriptor(dataId).annotation = dataAnnotationPtr;
}

void MBIPlotChart::AddVariable(const VarId &dataId)
{
    if (dataId != 0)
    {
        m_vargaph.insert(dataId);

        /* Force visibility */
        GetDataDescriptor(dataId).bHidden = false;
        GetDataDescriptor(dataId).bMoved = true;
    }
}

MBIPlotChart::VarId MBIPlotChart::CreateVariable(const MBISyncCircularBuffer<SpiedDataPoint> *const dataPtr, uint32_t period)
{
    DataRenderInfos *dataRender = nullptr;
    const VarId id = MakeUUID();

    /* Must alocate a new data renderer infos ? */
    if (m_varData.find(id) == m_varData.end())
    {
        dataRender = new DataRenderInfos(dataPtr);
        dataRender->dataPeriodMs = period;
        m_varData[id] = dataRender;
    }

    AddVariable(id);

    return id;
}

bool MBIPlotChart::IsVariableOnGraph(const VarId &dataId) const
{
    return (m_vargaph.find(dataId) != m_vargaph.end());
}

bool MBIPlotChart::IsVariableValid(const VarId &dataId) const
{
    return (dataId != 0);
}

void MBIPlotChart::SetDataDescriptorHandle(const VarId &dataId, DataDescriptorHandle dataRender)
{
    if (m_varData.find(dataId) != m_varData.end())
    {
        delete m_varData.find(dataId)->second;
    }
    m_varData[dataId] = new DataRenderInfos((const DataRenderInfos *const)dataRender);
}

MBIPlotChart::DataDescriptorHandle MBIPlotChart::GetDataDescriptorHandle(const VarId &dataId) const
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

const MBIPlotChart::DataRenderInfos &MBIPlotChart::GetDataDescriptor(const VarId &dataId) const
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

bool MBIPlotChart::DataDownSampled() const
{
    return m_downSampled;
}

MBIPlotChart::MBIPlotChart() : m_pause(true),
                               m_downSampled(false),
                               m_dsUpdate(true),
                               m_history(10.0),
                               m_activDownSampling(false),
                               m_xAxisRange{-10.0, 10.0}
{
    /* Create default invalid data descriptor */
    DataRenderInfos *dataRender = new DataRenderInfos(nullptr, false);
    dataRender->dataPeriodMs = 0;
    m_varData[0xFFFFFFFF] = dataRender;
}

MBIPlotChart::~MBIPlotChart()
{
    for (auto servData : m_varData)
        delete servData.second;
}

MBIPlotChart::DataRenderInfos &MBIPlotChart::GetDataDescriptor(const VarId &dataId)
{
    /* Force calling const impl by casting this as const */
    return const_cast<DataRenderInfos &>(const_cast<const MBIPlotChart *>(this)->GetDataDescriptor(dataId));
}

inline void MBIPlotChart::DisplayMarkers(UnitId unit)
{
    for (Marker &marker : m_markers)
    {
        const ImPlotDragToolFlags flags = marker.m_bstatic ? ImPlotDragToolFlags_NoInputs : ImPlotDragToolFlags_None;
        if (marker.m_bvisible)
        {
            if (marker.m_unit == unit)
            {
                if (unit == UNIT_TIME_X_AXIS)
                {
                    ImPlot::DragLineX(marker.m_id, (double *)&marker.m_value, marker.m_color, marker.m_thickness, flags);
                    if (strlen(marker.m_label) > 0)
                    {
                        ImPlot::Annotation(marker.m_value, 0, marker.m_color, ImVec2(0, 0), !marker.m_bstatic, marker.m_label);
                    }
                }
                else
                {
                    ImPlot::DragLineY(marker.m_id, (double *)&marker.m_value, marker.m_color, marker.m_thickness, flags);
                    if (strlen(marker.m_label) > 0)
                    {
                        ImPlot::Annotation(0, marker.m_value, marker.m_color, ImVec2(0, 0), !marker.m_bstatic, marker.m_label);
                    }
                }
            }
        }
    }
}

/**
 * @brief Data getter for graph plotting : converts DataRenderInfos into implot curve.
 * @warning This routine is called every frame, for each points of the curve. So this routine must be as short as possible !
 *
 * @param idx Index of current point
 * @param user_data Pointer on data infos
 * @return ImPlotPoint Point to plot
 */
ImPlotPoint MBIPlotChart::DataRenderInfos::PrtDataGetter(int idx, void *user_data)
{
    const MBIPlotChart::DataRenderInfos *const comdata = static_cast<MBIPlotChart::DataRenderInfos *>(user_data);
    const SpiedDataPoint &point = (*(comdata->data))[idx + comdata->dataOffset];
    return ImPlotPoint(point.m_time, point.m_data);
}
/**
 * @brief Data getter for down sampled data plotting : converts DataRenderInfos into implot curve.
 * @warning This routine is called every frame, for each points of the curve. So this routine must be as short as possible !
 *
 * @param idx Index of current point
 * @param user_data Pointer on data infos
 * @return ImPlotPoint Point to plot
 */
ImPlotPoint MBIPlotChart::DataRenderInfos::PrtDsDataGetter(int idx, void *user_data)
{
    const MBIPlotChart::DataRenderInfos *const comdata = static_cast<MBIPlotChart::DataRenderInfos *>(user_data);
    const SpiedDataPoint &point = comdata->dsData[idx];
    return ImPlotPoint(point.m_time, point.m_data);
}

int MBIPlotChart::DataRenderInfos::DownSampleLTTB(int start, int rawSamplesCount, int downSampleSize)
{
    // Largest Triangle Three Buckets (LTTB) Downsampling Algorithm
    //  "Downsampling time series for visual representation" by Sveinn Steinarsson.
    //  https://skemman.is/bitstream/1946/15343/3/SS_MSthesis.pdf
    //  https://github.com/sveinn-steinarsson/flot-downsample

    const double every = ((double)rawSamplesCount) / ((double)downSampleSize);
    int aIndex = 0;
    dsData.clear();
    dsData.reserve(downSampleSize);

    // fill first sample
    dsData.push_back(GetDataAt(start, 0));
    //   loop over samples
    for (int i = 0; i < downSampleSize - 2; ++i)
    {
        int avgRangeStart = (int)(i * every) + 1;
        int avgRangeEnd = (int)((i + 1) * every) + 1;
        if (avgRangeEnd > downSampleSize)
            avgRangeEnd = downSampleSize;

        const int avgRangeLength = avgRangeEnd - avgRangeStart;
        double avgX = 0.0;
        double avgY = 0.0;
        for (; avgRangeStart < avgRangeEnd; ++avgRangeStart)
        {
            SpiedDataPoint sample = GetDataAt(start, avgRangeStart);
            if (sample.m_data != NAN)
            {
                avgX += sample.m_time;
                avgY += sample.m_data;
            }
        }
        avgX /= (double)avgRangeLength;
        avgY /= (double)avgRangeLength;

        int rangeOffs = (int)(i * every) + 1;
        int rangeTo = (int)((i + 1) * every) + 1;
        if (rangeTo > downSampleSize)
            rangeTo = downSampleSize;
        SpiedDataPoint samplePrev = GetDataAt(start, aIndex);
        double maxArea = -1.0;
        int nextAIndex = rangeOffs;
        for (; rangeOffs < rangeTo; ++rangeOffs)
        {
            SpiedDataPoint sampleAtRangeOffs = GetDataAt(start, rangeOffs);
            if (sampleAtRangeOffs.m_data != NAN)
            {
                const double area = fabs((samplePrev.m_time - avgX) * (sampleAtRangeOffs.m_data - samplePrev.m_data) - (samplePrev.m_time - sampleAtRangeOffs.m_time) * (avgY - samplePrev.m_data)) / 2.0;
                if (area > maxArea)
                {
                    maxArea = area;
                    nextAIndex = rangeOffs;
                }
            }
        }
        dsData.push_back(GetDataAt(start, nextAIndex));
        aIndex = nextAIndex;
    }
    // fill last sample
    dsData.push_back(GetDataAt(start, rawSamplesCount - 1));
    return downSampleSize;
}