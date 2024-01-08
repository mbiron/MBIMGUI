#include <shobjidl.h>
#include <shlwapi.h>
#include <comdef.h>

#include "MBIFileDialog.h"

using namespace MBIMGUI;

constexpr wchar_t *DESC_NONE = L"";
constexpr wchar_t *DESC_ALL = L"All";

class CDialogEventHandler : public IFileDialogEvents,
                            public IFileDialogControlEvents
{
public:
    // IUnknown methods
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv)
    {
        static const QITAB qit[] = {
            QITABENT(CDialogEventHandler, IFileDialogEvents),
            QITABENT(CDialogEventHandler, IFileDialogControlEvents),
            {0},
#pragma warning(suppress : 4838)
        };
        return QISearch(this, qit, riid, ppv);
    }

    IFACEMETHODIMP_(ULONG)
    AddRef()
    {
        return InterlockedIncrement(&_cRef);
    }

    IFACEMETHODIMP_(ULONG)
    Release()
    {
        long cRef = InterlockedDecrement(&_cRef);
        if (!cRef)
            delete this;
        return cRef;
    }

    // IFileDialogEvents methods
    IFACEMETHODIMP OnFileOk(IFileDialog *) { return S_OK; };
    IFACEMETHODIMP OnFolderChange(IFileDialog *) { return S_OK; };
    IFACEMETHODIMP OnFolderChanging(IFileDialog *, IShellItem *) { return S_OK; };
    IFACEMETHODIMP OnHelp(IFileDialog *) { return S_OK; };
    IFACEMETHODIMP OnSelectionChange(IFileDialog *) { return S_OK; };
    IFACEMETHODIMP OnShareViolation(IFileDialog *, IShellItem *, FDE_SHAREVIOLATION_RESPONSE *) { return S_OK; };
    IFACEMETHODIMP OnTypeChange(IFileDialog *pfd) { return S_OK; };
    IFACEMETHODIMP OnOverwrite(IFileDialog *, IShellItem *, FDE_OVERWRITE_RESPONSE *) { return S_OK; };

    // IFileDialogControlEvents methods
    IFACEMETHODIMP OnItemSelected(IFileDialogCustomize *pfdc, DWORD dwIDCtl, DWORD dwIDItem) { return S_OK; };
    IFACEMETHODIMP OnButtonClicked(IFileDialogCustomize *, DWORD) { return S_OK; };
    IFACEMETHODIMP OnCheckButtonToggled(IFileDialogCustomize *, DWORD, BOOL) { return S_OK; };
    IFACEMETHODIMP OnControlActivating(IFileDialogCustomize *, DWORD) { return S_OK; };

    CDialogEventHandler() : _cRef(1){};

private:
    ~CDialogEventHandler(){};
    long _cRef;
};

MBIFileDialog::MBIFileDialog(EFileDialog eFileDialog) : m_bOpen(false), m_bStopThread(false), m_bSelected(false), m_pfd(NULL), m_pfde(NULL)
{
    DWORD dwFlags = 0;
    HRESULT hr = 0;
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (eFileDialog == MBI_E_FILE_OPEN_DLG)
    {
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pfd));
    }
    else
    {
        hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pfd));
    }
    if (SUCCEEDED(hr))
    {
        /* Create an event handling object, and hook it up to the dialog. */
        hr = CDialogEventHandler_CreateInstance(IID_PPV_ARGS(&m_pfde));
        if (SUCCEEDED(hr))
        {
            /* Hook up the event handler. */
            hr = m_pfd->Advise(m_pfde, &m_dwCookie);
            if (SUCCEEDED(hr))
            {
                /* Before setting, always get the options first in order not to override existing options. */
                hr = m_pfd->GetOptions(&dwFlags);
                if (SUCCEEDED(hr))
                {
                    /* Restrict view to file system items. */
                    hr = m_pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
                    if (SUCCEEDED(hr))
                    {
                        /* Start display thread */
                        m_DialogThread = std::thread(&MBIFileDialog::DialogThread, this);
                    }
                }
            }
        }
    }
    if (FAILED(hr))
    {
        throw _com_error(hr);
    }
}
MBIFileDialog::~MBIFileDialog()
{
    m_bStopThread = true;
    m_DialogThread.join();

    // Unhook the event handler.
    m_pfd->Unadvise(m_dwCookie);
    m_pfde->Release();
    m_pfd->Release();
}

void MBIFileDialog::SetTypeFilters(const std::vector<std::string> &filters)
{
    size_t len = (filters.size() > (NB_MAX_FILTERS - 1)) ? (NB_MAX_FILTERS - 1) : filters.size();
    std::wstring allstring = L"";

    m_FilterExtensions.clear();
    m_FilterExtensions.reserve(NB_MAX_FILTERS);
    for (int i = 0; i < len; i++)
    {
        /* Add wildcard at the beginning for file filter on filesystem */
        std::wstring wstr = L"*";
        wstr.append(filters[i].begin(), filters[i].end());
        /* Store a wchar version of the filter */
        m_FilterExtensions.push_back(wstr);
        allstring.append(wstr.append(L";"));

        m_SaveTypes[i].pszName = DESC_NONE;
        m_SaveTypes[i].pszSpec = m_FilterExtensions.back().c_str();
    }

    /* Create last entry containing all specified filters if many */
    if (len > 1)
    {
        m_FilterExtensions.push_back(allstring);

        m_SaveTypes[len].pszName = DESC_ALL;
        m_SaveTypes[len].pszSpec = m_FilterExtensions.back().c_str();
        len++;
    }

    // Set the file types to display only. Notice that, this is a 1-based array.
    HRESULT hr = m_pfd->SetFileTypes((UINT)m_FilterExtensions.size(), m_SaveTypes);
    if (SUCCEEDED(hr))
    {
        // Set the selected file type index to first given filter.
        hr = m_pfd->SetFileTypeIndex(len);
        if (SUCCEEDED(hr))
        {
            // Set the default extension to be ".doc" file.
            hr = m_pfd->SetDefaultExtension(m_SaveTypes[0].pszSpec);
        }
    }
    if (FAILED(hr))
    {
        throw _com_error(hr);
    }
}

void MBIFileDialog::SetTitle(const std::string &title) noexcept
{
    m_Title = std::wstring(title.begin(), title.end());
    m_pfd->SetTitle(m_Title.c_str());
}

void MBIFileDialog::SetInputName(std::string_view input) noexcept
{
    m_DefaultFilename = std::wstring(input.begin(), input.end());
    m_pfd->SetFileName(m_DefaultFilename.c_str());
}

void MBIFileDialog::SetPwd(const std::filesystem::path &pwd)
{
    IShellItem *pCurFolder = NULL;
    HRESULT hr = SHCreateItemFromParsingName(pwd.c_str(), NULL, IID_PPV_ARGS(&pCurFolder));
    if (SUCCEEDED(hr))
    {
        hr = m_pfd->SetDefaultFolder(pCurFolder);
        pCurFolder->Release();
    }
    if (FAILED(hr))
    {
        throw _com_error(hr);
    }
}

void MBIFileDialog::ClearSelected() noexcept
{
    m_FileName.clear();
    m_bOpen = false;
    m_bSelected = false;
}

HRESULT MBIFileDialog::CDialogEventHandler_CreateInstance(REFIID riid, void **ppv)
{
    *ppv = NULL;
    CDialogEventHandler *pDialogEventHandler = new (std::nothrow) CDialogEventHandler();
    HRESULT hr = pDialogEventHandler ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        hr = pDialogEventHandler->QueryInterface(riid, ppv);
        pDialogEventHandler->Release();
    }
    return hr;
}

void MBIFileDialog::DialogThread()
{
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    while (m_bStopThread == false)
    {
        if (m_bOpen)
        {
            // Show the dialog
            HRESULT hr = m_pfd->Show(NULL);
            /* If the user clicked on "Ok" */
            if (SUCCEEDED(hr))
            {
                // Obtain the result, once the user clicks the 'Open' button.
                // The result is an IShellItem object.
                IShellItem *psiResult;
                hr = m_pfd->GetResult(&psiResult);
                if (SUCCEEDED(hr))
                {
                    // Get the associated file name
                    PWSTR pszFilePath = NULL;
                    hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    if (SUCCEEDED(hr))
                    {
                        m_FileName = std::filesystem::path(pszFilePath);
                        m_bSelected = true;
                        m_bOpen = false;
                    }
                    /* Release the result */
                    psiResult->Release();
                }
            }
            /* If the user clicked on "cancel" */
            else if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
            {
                ClearSelected();
                hr = ERROR_SUCCESS;
            }
        }
    }
}