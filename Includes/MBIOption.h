

namespace MBIMGUI
{
    /**
     * @brief Private namespace handling option management.
     * @warning Should not be used directly ! Use @ref MBIMGUI::LoadOption and @ref MBIMGUI::SaveOption
     *
     */
    namespace MBIOPTMGR
    {
        /**
         * @brief Generic template for options value retreiving
         *
         * @tparam T Type of the option
         * @param str Value as a string
         * @param val Value as T
         */
        template <typename T>
        void ConvertOptionValue(std::string_view str, T *val);

        template <>
        inline void ConvertOptionValue(std::string_view str, int *val)
        {
            *val = std::stoi(str.data());
        }
        template <>
        inline void ConvertOptionValue(std::string_view str, size_t *val)
        {
            *val = std::stoull(str.data());
        }
        template <>
        inline void ConvertOptionValue(std::string_view str, double *val)
        {
            *val = std::stod(str.data());
        }
        template <>
        inline void ConvertOptionValue(std::string_view str, float *val)
        {
            *val = std::stof(str.data());
        }
        template <>
        inline void ConvertOptionValue(std::string_view str, std::string *val)
        {
            *val = str.data();
        }
        template <>
        inline void ConvertOptionValue(std::string_view str, bool *val)
        {
            // TODO : ugly
            *val = (str.data()[0] == '1');
        }

        /**
         * @brief Generic template for options value saving
         *
         * @tparam T Type of the option
         * @param val Value as T
         * @return std::string Value as std::string
         */
        template <typename T>
        inline std::string OptionToStr(T val)
        {
            return std::to_string(val);
        }

        template <>
        inline std::string OptionToStr(std::string val)
        {
            return val;
        }

        /**
         * @brief Store an option in RAM.
         * @warning Option will not be saved until MBIMGUI::WriteAllOptions has been called
         *
         * @param key Key of the option
         * @param val Value of the option
         */
        void WriteOption(std::string_view key, std::string_view val);
        /**
         * @brief Read the value of an option in RAM
         *
         * @param key Key of the option
         * @return const std::string& Value of the option as a string
         * @throw std::out_of_range if option doesn't exist
         */
        const std::string &ReadOption(std::string_view key);
    };

    /**
     * @brief Class describing a SW configuration options. This can be use to declare and store persistent options
     * for SW developped using MBIMGUI. Currently, options are stored in a file.
     *
     * @warning Keys used for options must be unique !
     *
     * @tparam T Type of the option
     */
    template <typename T>
    class MBIOption
    {
    private:
        std::pair<std::string, T> opt; ///< Option is a pair <key,value>

    public:
        MBIOption() = delete;
        /**
         * @brief Construct a new MBIOption object with the provided key.
         *
         * @param key Option key, must be unique.
         */
        explicit MBIOption(std::string_view key) noexcept
        {
            opt.first = key;
        }
        /**
         * @brief Construct a new MBIOption object with the provided key and value.
         *
         * @param key Option key, must be unique.
         * @param value Option value.
         */
        explicit MBIOption(std::string_view key, T value) noexcept : opt(key, value) {}

        /**
         * @brief Get the key of the option
         *
         * @return std::string_view Option key
         */
        std::string_view getKey() const noexcept
        {
            return opt.first;
        }
        /**
         * @brief Set the value of the option
         *
         * @param value New value of the option
         */
        void setValue(T value) noexcept
        {
            opt.second = value;
        }
        /**
         * @brief Get the value of the option
         *
         * @return T Current value of the option
         */
        T getValue() const noexcept
        {
            return opt.second;
        }
    };
}