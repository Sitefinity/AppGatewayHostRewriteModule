#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>

// Create the module's global class.
class HostRewriteModule : public CGlobalModule
{
public:

    // Process a GL_APPLICATION_START notification.
    GLOBAL_NOTIFICATION_STATUS
        OnGlobalPreBeginRequest(
            IN IPreBeginRequestProvider* pProvider
        )
    {
        UNREFERENCED_PARAMETER(pProvider);

        // Create an HRESULT to receive return values from methods.
        HRESULT hr;

        // Specify the "User-Agent" header name.
        char szHostHeaderName[] = "Host";

        // Create buffers to store the returned header values.
        PCSTR pszXOriginalHost;

        // Create buffers to store lengths of the returned header values.
        USHORT cchXOriginalHostLength;

        IHttpContext* pHttpContext = pProvider->GetHttpContext();
        IHttpRequest* pHttpRequest = pHttpContext->GetRequest();

        if (pHttpRequest != NULL)
        {
            // Look for the "User-Agent" header.
            pszXOriginalHost = pHttpRequest->GetHeader("X-Original-Host", &cchXOriginalHostLength);

            // The header length will be 0 if the header was not found.
            if (cchXOriginalHostLength != 0)
            {
                // Allocate space to store the header.
                pszXOriginalHost = (PCSTR)pHttpContext->AllocateRequestMemory(cchXOriginalHostLength + 1);

                // Test for an error.
                if (pszXOriginalHost != NULL)
                {
                    pszXOriginalHost = pHttpRequest->GetHeader("X-Original-Host", &cchXOriginalHostLength);

                    // Test for an error.
                    if (pszXOriginalHost != NULL)
                    {
                        // Replace the "Host" header.
                        hr = pHttpRequest->SetHeader(szHostHeaderName, pszXOriginalHost, (USHORT)strlen(pszXOriginalHost), true);

                        size_t i;
                        mbstowcs_s(&i, nullptr, 0, pszXOriginalHost, _TRUNCATE);

                        wchar_t* httpHost = (wchar_t*)malloc(i * 2);
                        mbstowcs_s(&i, httpHost, i, pszXOriginalHost, _TRUNCATE);

                        // Replace the HTTP_HOST server variable
                        hr = pHttpContext->SetServerVariable("HTTP_HOST", httpHost);
                    }
                }
            }
        }

        // Return processing to the pipeline.
        return GL_NOTIFICATION_CONTINUE;
    }

    VOID Terminate()
    {
        delete this;
    }
};

// Create the module's exported registration function.
HRESULT
__stdcall
RegisterModule(
    DWORD dwServerVersion,
    IHttpModuleRegistrationInfo* pModuleInfo,
    IHttpServer* pGlobalInfo
)
{
    UNREFERENCED_PARAMETER(dwServerVersion);
    UNREFERENCED_PARAMETER(pGlobalInfo);

    // Create an instance of the global module class.
    HostRewriteModule* pGlobalModule = new HostRewriteModule;
    // Test for an error.
    if (NULL == pGlobalModule)
    {
        return HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
    }
    // Set the global notifications and exit.
    return pModuleInfo->SetGlobalNotifications(
        pGlobalModule, GL_PRE_BEGIN_REQUEST);
}