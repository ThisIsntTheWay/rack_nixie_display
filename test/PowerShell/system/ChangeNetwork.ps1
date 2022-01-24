$t = @"
{
	"ipConfig": {
        "useDHCP": false,
		"deviceIP": "192.168.1.195",
		"netmask": "255.255.255.0",
		"gateway": "192.168.1.1",
        "dns": "192.168.1.1"
	}
}
"@

$deviceIP = "192.168.1.118"
$uri = "http://$deviceIP/api/network"
Invoke-WebRequest $uri -method POST -contentType application/json -body $t