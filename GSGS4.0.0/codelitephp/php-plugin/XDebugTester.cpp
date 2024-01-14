#include "XDebugTester.h"
#include "gsgsPathManager.h"
#include "gsgsZip.h"
#include "phpexecutor.h"
#include "gsgsJSON.h"
#include "php_configuration_data.h"

XDebugTester::XDebugTester() {}

XDebugTester::~XDebugTester() {}

bool XDebugTester::RunTest()
{
    // Get the path to the xdebug tester script
    wxFileName xdebugTesterScript(gsgs_Path().GetUserDataDir(), "TestXDebugSettings.php");
    if(!xdebugTesterScript.Exists()) {
        gsgs::ZipReader zipReader(wxFileName(gsgs_Path().GetDataDir(), "PHP.zip"));
        zipReader.Extract(xdebugTesterScript.GetFullName(), xdebugTesterScript.GetPath());
    }

    if(xdebugTesterScript.Exists()) {
        PHPConfigurationData globalConf;
        globalConf.Load();

        PHPExecutor executor;
        wxString php_output;
        if(executor.RunScript(xdebugTesterScript.GetFullPath(), php_output)) {
            JSON root(php_output);
            JSONObject rootElement = root.getObject();
            wxString tempstr;
            //////////////////////////////////////////////////
            // Directives
            //////////////////////////////////////////////////
            {
                wxString msg;

                rootElement.getObject("_remoteConnectBack").getValue(tempstr);
                if(tempstr != "1") {
                    msg << "<font color=\"red\">Failed. This value should be set to 1</font>";
                } else {
                    msg << "<font color=\"green\">Passed</font>";
                }
                rootElement.getObject("_remoteConnectBack").getValue(tempstr);
                m_results.insert(
                    std::make_pair(wxString("xdebug.remote_connect_back"),
                                   std::make_pair(tempstr, msg)));
            }
            {
                wxString msg;

                rootElement.getObject("_ideKey").getValue(tempstr);
                if(tempstr != globalConf.GetXdebugIdeKey()) {
                    msg << "<font color=\"red\">Failed. This value should be set to \"" << globalConf.GetXdebugIdeKey()
                        << "\"</font>";
                } else {
                    msg << "<font color=\"green\">Passed</font>";
                }
                rootElement.getObject("_ideKey").getValue(tempstr);
                m_results.insert(std::make_pair(wxString("xdebug.idekey"),
                                                std::make_pair(tempstr, msg)));
            }
            rootElement.getObject("_remotePort").getValue(tempstr);
            m_results.insert(std::make_pair(
                wxString("xdebug.remote_port"),
                std::make_pair(tempstr,
                               "The port to which Xdebug tries to connect on the remote host. The default is 9000")));

            rootElement.getObject("_remoteHost").getValue(tempstr);
            m_results.insert(std::make_pair(wxString("xdebug.remote_host"),
                                            std::make_pair(tempstr,
                                                           "Selects the host where the debug client is running\nYou "
                                                           "can either use a host name or an IP address.\nThis setting "
                                                           "is ignored if xdebug.remote_connect_back is enabled")));
            {
                wxString msg;
                rootElement.getObject("_remoteEnable").getValue(tempstr);
                if(tempstr != "1") {
                    msg << "<font color=\"red\">Failed. This value should be set to 1</font>";
                } else {
                    msg << "<font color=\"green\">Passed</font>";
                }
                rootElement.getObject("_remoteEnable").getValue(tempstr);
                m_results.insert(
                    std::make_pair(wxString("xdebug.remote_enable"),
                                   std::make_pair(tempstr, msg)));
            }
            {
                // XDebug loaded
                wxString msg;
                wxString temp;
                rootElement.getObject("_xdebugLoaded").getValue(temp);
                if(temp != "1") {
                    msg << "<font color=\"red\">Failed. XDebug is NOT loaded</font>";
                } else {
                    msg << "<font color=\"green\">Passed</font>";
                }
                rootElement.getObject("_xdebugLoaded").getValue(temp);
                m_results.insert(
                    std::make_pair(wxString("XDebug Loaded"),
                                   std::make_pair(temp, msg)));
            }

            // Zend Debugger loaded
            {
                // If Zend Debugger is loaded, mark it the message with RED
                wxString msg;
                wxString temp;
                rootElement.getObject("_zendDebuggerLoaded").getValue(temp);
                if(temp == "1") {
                    msg << "<font color=\"red\">Failed. Unload Zend Debugger extension"
                           "</font>";
                    rootElement.getObject("_zendDebuggerLoaded").getValue(temp);
                    m_results.insert(
                        std::make_pair(wxString("Zend Debugger Loaded"),
                                       std::make_pair(temp, msg)));
                }
            }
            return true;
        }
    }
    return false;
}
