#pragma once

#include <unordered_set>
#include <map>
#include "implot.h"

/**
 * @brief Struct describing a data occurence. It's basically a value with the corresponding date.
 */
struct DataPoint
{
    double m_time; ///< Time in s
    double m_data; ///< Data value

    /**
     * @brief Construct a new Spied Data Point object
     *
     * @param x x-axis data : time
     * @param y y-axis data : value
     */
    explicit DataPoint::DataPoint(double x = 0, double y = 0) noexcept : m_time(x),
                                                                         m_data(y)
    {
    }
};

/**
 * @brief Struct describing an annotation displayed on a graph. You shall derived this class and implement
 * DataAnnotation::GetLabel to create your own specific annotation on the graph.
 *
 */
struct DataAnnotation
{
    double m_x;             ///< Time in s
    double m_y;             ///< Data value
    uintptr_t m_annotInfos; ///< User specific data

    /**
     * @brief Construct a new Data Annotation object
     *
     * @param x X-Axis annotation position
     * @param y Y-Axis annotation position
     * @param infos User specific info. You can set and use this to store specifc annotation data.
     */
    explicit DataAnnotation(double x = 0, double y = 0, uintptr_t infos = 0) : m_x(x), m_y(y), m_annotInfos(infos) {}

    /**
     * @brief This method is called on each annotation displayed on the graph. Override it to defined your own annotation on graph.
     *
     * @return const char* Annotation
     */
    virtual const char *GetLabel() const noexcept { return ""; };
    /**
     * @brief This method is called on each annotation displayed on the graph. Override it to defined your own annotation on graph.
     * Otherwise, the default color is white.
     *
     * @return const ImVec4 Color of the annotation
     */
    virtual const ImVec4 GetColor() const noexcept { return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); };
};

/**
 * @brief Describe a variable displayed on the graph
 *
 */
struct DataDescriptor
{
    using UnitId = uint8_t;
    using DataUnit = std::pair<UnitId, std::string>;
    static constexpr UnitId INVALID_UNIT = ((UnitId)-1); ///< Invalid unit, only for init value

    bool bShowAnnotations; ///< Show annotations on the graph for the current variable
    bool bHidden;          ///< Data showned on the graph ?
    bool bMoved;           ///< True if the data has just been moved from a graph to another. Used to force data visibility after dnd
    ImVec4 color;          ///< Color of the curve
    std::string name;      ///< Name of the curve
    DataUnit unit;         ///< Unit of the variable. Use to determine the axis
    ImAxis axis;           ///< Current axis used to display data

    explicit DataDescriptor(bool showLabels = false) : bShowAnnotations(showLabels),
                                                       bHidden(false),
                                                       bMoved(false),
                                                       color(255, 255, 255, 255),
                                                       name(""),
                                                       unit(INVALID_UNIT, ""),
                                                       axis(ImAxis_COUNT)
    {
    }
};

/**
 * @brief Define a curve displayed on the graph
 *
 */
template <template <typename> class Container>
struct DataRenderInfos
{
public:
    const Container<DataPoint> *const data;                  ///< Curve data points
    ImVector<DataPoint> dsData;                              ///< Down sampled curve data
    const MBISyncCircularBuffer<DataAnnotation> *annotation; ///< Data annotation, if exists

    uint32_t dataOffset;       ///< Start display offset of data
    uint32_t dataPeriodMs;     ///< Sampling data period in ms
    DataDescriptor descriptor; ///< Curve descriptor

    /**
     * @brief Construct a new DataRenderInfos object
     *
     * @param showLabels Show annotations on the graph ?
     */
    explicit DataRenderInfos(const Container<DataPoint> *const ptrData, bool showLabels = false) : data(ptrData),
                                                                                                   dataOffset(0),
                                                                                                   dataPeriodMs(1),
                                                                                                   descriptor(showLabels),
                                                                                                   annotation(nullptr)

    {
    }

    /**
     * @brief Copy construct a new Data Render Infos object
     *
     * @param other
     */
    explicit DataRenderInfos(const DataRenderInfos *const other) noexcept : data(other->data),
                                                                            descriptor(other->descriptor),
                                                                            dataOffset(0),
                                                                            dataPeriodMs(other->dataPeriodMs),
                                                                            annotation(other->annotation)
    {
    }

    /**
     * @brief Apply LTTB down sampling algorithm to data and store result sampled data in dsData.
     * Taken from https://github.com/epezent/implot/pull/389/commits/cf3e4a76bd8fea7dd067e2acd591d2365edb3c0d
     * slightly modified by me
     *
     * @param start Offset of the data to display
     * @param rawSamplesCount Total size of origin data samples
     * @param downSampleSize Down sample size
     * @return int Size of dsData.
     */
    int DownSampleLTTB(int start, int rawSamplesCount, int downSampleSize)
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
                DataPoint sample = GetDataAt(start, avgRangeStart);
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
            DataPoint samplePrev = GetDataAt(start, aIndex);
            double maxArea = -1.0;
            int nextAIndex = rangeOffs;
            for (; rangeOffs < rangeTo; ++rangeOffs)
            {
                DataPoint sampleAtRangeOffs = GetDataAt(start, rangeOffs);
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

private:
    inline const DataPoint &GetDataAt(int offset, int idx) const
    {
        return (*data)[offset + idx];
    }
};

/**
 * @brief Generic plot chart with time as x-axis, multiple variables visualization, LTTB downsampling,
 * markers and 3 y-axis units available.
 *
 */
class MBIPlotChart
{
public:
    using VarId = uint32_t;                    ///< Variable unique identifier. Used as a handle for displayed variables.
    using DataContainer = ImVector<DataPoint>; ///< Displayed variable data points.
    using UnitId = DataDescriptor::UnitId;     ///< Variable unit unique identifier. Unit defines the y-axis on which the variable shall be drawn. Multiples variables sharing the same unit are drawn on the same axis.
    using DataUnit = DataDescriptor::DataUnit;
    using DataDescriptorHandle = const void *const;
    using MBIDndCb = void (*)(void *, const ImGuiPayload *);

    static constexpr UnitId UNIT_NONE = ((UnitId)0);                ///< No unit, means no label will be displayed. All variables without unit are on the same axis
    static constexpr UnitId UNIT_USER_MIN = ((UnitId)1);            ///< Start of the user-defined units. Use this as an enum start value
    static constexpr UnitId UNIT_USER_MAX = ((UnitId)99);           ///< End of the user-defined units. Use this as an enum start value
    static constexpr UnitId UNIT_TIME_X_AXIS = ((UnitId)100);       ///< Useful for vertical markers
    static constexpr char *DND_LABEL_FROM_GRAPH = "ParamFromGraph"; ///< For graph to graph DND

    /**
     * @brief Class defining a marker to be drawn on graph
     *
     */
    struct Marker
    {
        int m_id;          ///< Unique marker Id
        bool m_bvisible;   ///< Is the marker visible
        bool m_bstatic;    ///< Is the marker moveable by user
        double m_value;    ///< Value of the marker
        ImVec4 m_color;    ///< Color of the marker
        float m_thickness; ///< Thickness of the marker
        char m_label[100]; ///< Label of the marker
        UnitId m_unit;     ///< Marker axis

        /**
         * @brief Construct a new Marker object
         *
         * @param id Must be unique ! Two markers can't have the same Id.
         * @param value Value of the marker
         */
        explicit Marker(int id = 0, float value = 0.0f)
            : m_id(id),
              m_value(value),
              m_bvisible(true),
              m_bstatic(false),
              m_color(255, 255, 255, 255),
              m_thickness(1.0),
              m_label(""),
              m_unit(UNIT_NONE)
        {
        }

        /**
         * @brief Comparison operator. Needed for std::list::remove()
         *
         * @param other
         * @return true
         * @return false
         */

        bool operator==(const Marker &other) const
        {
            return this->m_id == other.m_id;
        };
    };

    /***********************************************************
     *
     *  CTOR & DTOR
     *
     * *********************************************************/

    /**
     * @brief Construct a new MBIPlotChart object
     *
     */
    explicit MBIPlotChart(ImPlotScale xAxisScale = ImPlotScale_Linear);

    /**
     * @brief Destroy the MBIPlotChart object
     *
     */
    ~MBIPlotChart();

    /***********************************************************
     *
     *  Variables management
     *
     * *********************************************************/

    /**
     * @brief Add the given variable on the plot
     *
     * @param dataId Identifier of the data to add
     */
    void AddVariable(const VarId &dataId);

    /**
     * @brief Create a variable object and add it to the plot.
     *
     * @param dataPtr Data of the variable
     * @param period Period of the data in ms. Set to zero for non periodic data
     * @return VarId Variable identifier to be used for other functions calls.
     */
    VarId CreateVariable(const DataContainer *const dataPtr, uint32_t period = 0);

    /**
     * @brief Remove the specified variable from the plot
     *
     * @param dataId Variable identifier
     * @return true If the variable has been removed
     * @return false If the variable doesn't exists on the plot
     */
    bool RemoveVariable(const VarId &dataId);

    /**
     * @brief Detects if the given variable is on the plot
     *
     * @param dataId  Identifier of the variable
     * @return true If the variable is on the plot
     * @return false If the variable is not on the plot
     */
    bool IsVariableOnGraph(const VarId &dataId) const;

    /**
     * @brief Verify if the given variable identifier is valid and can be used for this plot.
     *
     * @param dataId Identifier of the variable
     * @return true The variable id is valid
     * @return false The variable id is not valid
     */
    bool IsVariableValid(const VarId &dataId) const noexcept;

    /**
     * @brief Set the Data Descriptor Handle for the given variable. This method is useful when moving a variable from another plot.
     *
     * @param dataId Identifier of the variable
     * @param dataRender Data Descriptor Handle for the variable
     */
    virtual void SetDataDescriptorHandle(const VarId &dataId, DataDescriptorHandle dataRender);

    /**
     * @brief Get the Data Descriptor Handle object for the given variable. This method is useful when moving a variable to another plot.
     *
     * @param dataId Identifier of the variable
     * @return DataDescriptorHandle Data Descriptor Handle for the variable
     */
    virtual DataDescriptorHandle GetDataDescriptorHandle(const VarId &dataId) const;

    /**
     * @brief Set the variable displayed name
     *
     * @param dataId Variable identifier
     * @param name Name of the variable to be displayed
     */
    void SetVarName(const VarId &dataId, const std::string_view name);

    /**
     * @brief Set the variable plot line color
     *
     * @param dataId Variable identifier
     * @param color Color of the variable plot line
     */
    void SetVarColor(const VarId &dataId, const ImVec4 &color);

    /**
     * @brief Set the variable unit. The y-axis for the variable depends on its unit.
     *
     * @param dataId Variable identifier
     * @param unit Unit identifier
     */
    void SetVarUnit(const VarId &dataId, const DataUnit &unit);

    /***********************************************************
     *
     *  Markers management
     *
     * *********************************************************/

    /**
     * @brief Get the markers list as a non mutable reference
     *
     * @return const std::list<Marker>& List of the markers of the graph
     */
    const std::list<Marker> &GetMarkersList() const noexcept;

    /**
     * @brief Get the markers list. List can be modified by the caller.
     *
     * @return std::list<Marker>& List of the markers of the graph
     */
    std::list<Marker> &GetMarkersList() noexcept;

    /**
     * @brief Add the given marker on the plot
     *
     * @param marker Plot to add
     */
    void AddMarker(const Marker &marker);

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
    virtual void AddDataAnnotations(const VarId &dataId, const MBISyncCircularBuffer<DataAnnotation> *const dataAnnotationPtr);

    /**
     * @brief Enable of disable the annotations on the plot for the given variable
     *
     * @param dataId Variable identifier
     * @param activ True : annotations are shown
     *              False : annotations are hidden
     */
    void ToggleVarAnnotation(const VarId &dataId, bool activ);

    /***********************************************************
     *
     *  Axis
     *
     * *********************************************************/

    /**
     * @brief Get the axis range for the given unit (Y axis)
     *
     * @param eUnit Unit identifier of the axis
     * @return const ImPlotRange& Range of the axis
     */
    const ImPlotRange &GetYAxisRange(const UnitId &eUnit) const;

    /**
     * @brief Get the X axis range (time)
     *
     * @return const ImPlotRange& Range of the axis
     */
    const ImPlotRange &GetXAxisRange() const noexcept;

    /**
     * @brief Set the X axis range (time)
     *
     * @param range Range of the axis
     */
    void SetXAxisRange(const ImPlotRange &range) noexcept;

    /***********************************************************
     *
     *  Downsampling
     *
     * *********************************************************/

    /**
     * @brief Enable or disable down sampling of data for the graph
     *
     * @param bActiv True : Downsampling is enable.
     *               False : Downsampling is disable.
     * @param size Size of the downsampling window (number of samples)
     */
    void SetDownSampling(bool bActiv, size_t size) noexcept;

    /**
     * @brief Are data currently downsampled on the graph. Downsampling is activated if number of points to be displayed is greater than down sampling window.
     *
     * @return true Data are downsampled
     * @return false Data are not downsampled
     */
    bool DataDownSampled() const noexcept;

    /***********************************************************
     *
     *  Drag and Drop
     *
     * *********************************************************/

    /**
     * @brief Register a Drag and Drop callback for the given type
     *
     * @param type Label of the type of data to receive (used by SetDragDropPayload)
     * @param callback Function to be called when data are dropped
     */
    void SetDnDCallback(std::string_view type, MBIPlotChart::MBIDndCb callback, void *arg) noexcept;

    /***********************************************************
     *
     *  Main
     *
     * *********************************************************/

    /**
     * @brief Main function displaying the plot.
     *
     * @param label Title of the plot, must be unique.
     * @param size Size of the plot area.
     */
    virtual void Display(std::string_view label, ImVec2 size);

    /**
     * @brief Reset plot removing marker.
     *
     */
    void Reset() noexcept;

protected:
    ImAxis GetYAxisOffset(const UnitId &eUnit) const;

    bool m_downSampled; ///< Are displayed data currently down sampled ?
    bool m_dsUpdate;    ///< DownSampling must be recalculated

    bool m_activDownSampling;  ///< Is downsampling activated
    size_t m_downSamplingSize; ///< Downsampling size

    ImPlotScale m_xAxisScale;    ///< Type of X-axis
    ImPlotRange m_xAxisRange;    ///< X-axis range
    ImPlotRange m_yAxesRange[3]; ///< Y-axis range

    MBIDndCb m_callback;   ///< Pointer on the function to be called when data are dropped
    void *m_callbackArg;   ///< Argument to pass to the function
    std::string m_dndType; ///< Type of the data to accept

    std::unordered_set<VarId> m_vargaph; ///< Id of the variables currently displayed on the graph
    std::list<Marker> m_markers;         ///< List of the markers to be displayed on the graph

    static VarId MakeUUID()
    {
        static VarId cnt = 0;
        return ++cnt;
    }

    /**
     * @brief Display the markers for the given axis
     *
     * @param unit Axis of the markers
     */
    void DisplayMarkers(UnitId unit);

private:
    using DataRender = DataRenderInfos<ImVector>;

    std::map<uint32_t, DataRender *> m_varData; ///< Map containing the data to be displayed on the graphs

    void MBIPlotChart::ComputeDataWindow(DataRender &dataRenderInfos, size_t &dataSize, int32_t &dataOffset);

    DataRender &GetDataRenderInfos(const VarId &dataId);
    const DataRender &GetDataRenderInfos(const VarId &dataId) const;

    virtual const DataDescriptor &GetDataDescriptor(const VarId &dataId) const;
    virtual DataDescriptor &GetDataDescriptor(const VarId &dataId);
};