# Overkiz box without cloud connection
For using the overkiz box without cloud connection only the following processes are required:
* io-homecontrold
* locald
* lighttpd

## Start the protocol stacks without internald
Nevertheless io-homecontrold does not startup until a DBUS method call to ```com.overkiz.Application.Internal.Modules.getModules``` returns its name.  
Normally that is provided by the process internald, which starts a bunch of other process and connects to the cloud.  
For a pure offline use this process can be used instead. It takes as parameters the the modules which should be returned.  
E.g. ```/apps/overkiz-modules IoHomecontrol OVP```
(OVP is belongs also the io-homecontrold)

## Local API
BTW: If you want to access the local API the easiest way is placing a file in ```/etc/lighttpd.d``` with the following contents:
```
$SERVER["socket"] == ":1080" {
    ssl.engine  = "disable"
    setenv.add-request-header = ("X-OVK-Token-Scope" => "local")
    include_shell "token=\"$(dbus-send --print-reply --system --type=method_call --dest=com.overkiz.utilities.local /com/overkiz/utilities/local/tokensService com.overkiz.utilities.local.tokensService.createWithRandomKey string:test string:local int32:2147483648 )\"; token=${token##* \\"}; token=${token%%\\"*}; echo \"setenv.add-request-header += ( \\"X-Auth-Token\\" => \\"${token}\\" )\" "
    server.max-keep-alive-idle = 90
}
```
That will add a port http port 1080 and automatically add the security token header.

If you don't want to compile the process yourseld I also placed a binary in this reposity, which was compiled using OpenWRT
