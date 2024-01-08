#pragma once

#include <shlobj.h>
#include <vector>
#include <thread>
#include <string>
#include <filesystem>

namespace MBIMGUI
{
    constexpr size_t NB_MAX_FILTERS = 10; ///< Maximum number of filter
    /**
     * @brief Available file browser type
     *
     */
    typedef enum
    {
        MBI_E_FILE_OPEN_DLG, ///< Open file dialog : no new file creation
        MBI_E_FILE_SAVE_DLG  ///< Save file dialog : create new file
    } EFileDialog;

    /**
     * @brief Simple Windows-only file browser, allowing file open and file creation, filtering and default path selection.
     * After creation, the MBIFileDialog must be configured before calling MBIFileDialog::Open method to display the file explorer.
     * The method MBIFileDialog::HasSelected must be used to detect user choice, then MBIFileDialog::GetSelected to retreive selected file.
     * Finally, the MBIFileDialog::ClearSelected method shall be called after MBIFileDialog::GetSelected to reset browser for next Open call.
     *
     * The filebrowser implementation is based on official MSDN example : https://github.com/microsoft/Windows-classic-samples/blob/main/Samples/Win7Samples/winui/shell/appplatform/commonfiledialog/CommonFileDialogApp.cpp
     */
    class MBIFileDialog
    {
    public:
        /**
         * @brief Construct a new MBIFileDialog object. The file
         *
         * @param eFileDialog Type of the file browser to create
         */
        explicit MBIFileDialog(EFileDialog eFileDialog = MBI_E_FILE_OPEN_DLG);
        /**
         * @brief Destroy the MBIFileDialog object
         *
         */
        ~MBIFileDialog();
        /**
         * @brief Set the Type Filters for the object. The filters are strings of format ".ext". No more than NB_MAX_FILTERS can be setted.
         *
         * @param filters Vector of filters strings
         */
        void SetTypeFilters(const std::vector<std::string> &filters);
        /**
         * @brief Open the file dialog
         *
         */
        inline void Open() noexcept
        {
            m_bOpen = true;
        }
        /**
         * @brief Indicate if user has selected a file (file selection and "Open" or "Save" dialog button has been clicked)
         *
         * @return true File selected
         * @return false No file selected
         */
        inline bool HasSelected() const noexcept
        {
            return m_bSelected;
        }
        /**
         * @brief Set the Title of the dialog window
         *
         * @param title Title of the window
         */
        void SetTitle(const std::string &title) noexcept;
        /**
         * @brief Set the default path of the file dialog. This is the directory the file explorer dialog shows at first Open call.
         * Afterward, the last used directory is used as the new default directory.
         *
         * @param pwd Path to the default directory
         */
        void SetPwd(const std::filesystem::path &pwd);
        /**
         * @brief Set the default file name to be used in file explorer dialog.
         *
         * @param input Default filename
         */
        void SetInputName(std::string_view input) noexcept;
        /**
         * @brief Clear the current user selection.
         * @warning Must be called after HasSelected has return true to prepare next Open call.
         *
         */
        void ClearSelected() noexcept;
        /**
         * @brief Get the Selected file
         *
         * @return std::filesystem::path Path to the selected file
         */
        inline std::filesystem::path GetSelected() const noexcept
        {
            return m_FileName;
        }

    private:
        /**
         * @brief This function create the Dialog Event Handler object for the FileDialog explorer. This handler can be used for UI specific event related
         * actions (file selection, multiple selection, file creation etc.). See MSDN for more infos.
         *
         * @param riid Id of the IFileDialog object
         * @param ppv Address of the IFileDialog object
         * @return HRESULT Result of the object creation
         */
        HRESULT CDialogEventHandler_CreateInstance(REFIID riid, void **ppv);

        /**
         * @brief Thread displaying the FileDialog.
         *
         */
        void DialogThread();

        bool m_bOpen;       ///< Is dialog currently open
        bool m_bSelected;   ///< True if file has been selected
        bool m_bStopThread; /// Thread running flag

        /* Following parameters are stored to keep valid context for IFileDialog object */
        std::wstring m_Title;                         ///< Title of the window
        std::wstring m_DefaultFilename;               ///< File name displayed by default
        std::filesystem::path m_FileName;             ///< Path to the file choose by user
        std::vector<std::wstring> m_FilterExtensions; ///< List of file filters in wstring format

        std::thread m_DialogThread; ///< Handle of the DialogThread
        IFileDialog *m_pfd;         ///< File dialog object
        IFileDialogEvents *m_pfde;  ///< File dialog event handler object
        DWORD m_dwCookie;           ///< Cookie for handler reference

        COMDLG_FILTERSPEC m_SaveTypes[NB_MAX_FILTERS]; ///< Filterspec object for dialog filters
    };
}