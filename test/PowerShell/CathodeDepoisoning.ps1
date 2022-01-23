<#
    This little script "depoisons" the cathodes of all tubes by shifting through all digits.
#>

$ProgressPreference = 'SilentlyContinue'
$targetIP = "192.168.1.176"
$uri = "http://$targetIP/api/display"

function Create-RequestBody([int]$num) {
    return @{
        "tubes" = @{
            "1" = @{ "val" = $num; "pwm" = 255 }
            "2" = @{ "val" = $num; "pwm" = 255 }
            "3" = @{ "val" = $num; "pwm" = 255 }
            "4" = @{ "val" = $num; "pwm" = 255 }
        }
    } | convertto-json
}

# ---------------------------------------------------------------------------------------------
Invoke-RestMethod -uri $uri -Method POST -contenttype application/json -body (@{
        "onboardLed" = @{ "mode" = 3}
        "indicators" = @{ "1" = $false; "2" = $false }
    } | ConvertTo-Json)


while ($true) {
    for ($i = 0; $i -le 9; $i++) {
        $i; Invoke-RestMethod -Method POST -uri $uri -ContentType application/json -body (Create-RequestBody $i) | out-null
    }
    for ($i = 8; $i -gt 0; $i--) {
        $i; Invoke-RestMethod -Method POST -uri $uri -ContentType application/json -body (Create-RequestBody $i) | out-null
    }
}
