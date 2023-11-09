#pragma once

#include <unordered_set>
#include <map>
#include "MBISyncCircularBuffer.h"
#include "MBIPlotChart.h"

/**
 * @brief Generic real time plot chart with time as x-axis, multiple variables visualization, LTTB downsampling,
 * markers and 3 y-axis units available.
 *
 */
class MBIRealtimePlotChart : public MBIPlotChart
{
public:
    using DataContainer = MBISyncCircularBuffer<DataPoint>;
    using DataRender = DataRenderInfos<MBISyncCircularBuffer>;

    /***********************************************************
     *
     *  CTOR & DTOR
     *
     * *********************************************************/

    /**
     * @brief Construct a new MBIPlotChart object
     *
     */
    MBIRealtimePlotChart();

    /**
     * @brief Destroy the MBIPlotChart object
     *
     */
    ~MBIRealtimePlotChart();

    /***********************************************************
     *
     *  Variables management
     *
     * *********************************************************/

    /**
     * @brief Create a variable object and add it to the plot.
     *
     * @param dataPtr Data of the variable
     * @param period Period of the data in ms. Set to zero for non periodic data
     * @return VarId Variable identifier to be used for other functions calls.
     */
    VarId CreateVariable(const DataContainer *const dataPtr, uint32_t period = 0);

    /**
     * @brief Set the Data Descriptor Handle for the given variable. This method is useful when moving a variable from another plot.
     *
     * @param dataId Identifier of the variable
     * @param dataRender Data Descriptor Handle for the variable
     */
    void SetDataDescriptorHandle(const VarId &dataId, DataDescriptorHandle dataRender) override;

    /**
     * @brief Get the Data Descriptor Handle object for the given variable. This method is useful when moving a variable to another plot.
     *
     * @param dataId Identifier of the variable
     * @return DataDescriptorHandle Data Descriptor Handle for the variable
     */
    DataDescriptorHandle GetDataDescriptorHandle(const VarId &dataId) const override;

    /***********************************************************
     *
     *  Main
     *
     * *********************************************************/
    /**
     * @brief Set running state the plot. If paused, the x-axis will stop scrolling.
     *
     * @param pause True : The plot is paused.
     *              False : The plot is running
     */
    void Pause(bool pause);

    /**
     * @brief Set the history depth
     *
     * @param history History depth in seconds
     */
    void SetHistory(float history);

    /**
     * @brief Main function displaying the plot.
     *
     * @param currentTimeS Current time in seconds
     */
    void Display(double currentTimeS);
    
    /***********************************************************
     *
     *  Annotations
     *
     * *********************************************************/

    /**
     * @brief Add annotations for the variable
     *
     * @param dataId Identifier of the variable
     * @param dataAnnotationPtr Annotations to add
     */
    void AddDataAnnotations(const VarId &dataId, const MBISyncCircularBuffer<DataAnnotation> *const dataAnnotationPtr) override;

private:
    std::map<uint32_t, DataRender *> m_varData; ///< Map containing the data to be displayed on the graphs

    bool m_pause;    ///< Pause state for the graphs
    float m_history; ///< x-axis size (time history for data)

    DataDescriptor &GetDataDescriptor(const VarId &dataId) override;
    const DataDescriptor &GetDataDescriptor(const VarId &dataId) const override;

    DataRender &GetDataRenderInfos(const VarId &dataId);
    const DataRender &GetDataRenderInfos(const VarId &dataId) const;

    void ComputeDataWindow(DataRender &dataRenderInfos, size_t &dataSize, int32_t &dataOffset);
};