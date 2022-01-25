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
        "Indicators" = @{ "1" = $false; "2" = $false }
    } | ConvertTo-Json) | out-null

# (<tubeDigit>, <illuminationTime>)
$digitConfig = @((1, 0.8), (2, 1.5), (3, 1.5), (4, 0.8), (5, 0.1), (6, 0.1), (7, 0.3), (8, 0.1), (9, 0.3))
while ($true) {
    for ($i = 0; $i -le 9; $i++) {
        $digit = $digitConfig[$i][0]
        $sleepTime = [double]$digitConfig[$i][1] * 60

        Write-Host "$(get-date -f "[HH:mm:ss]") - Digit: '$digit' | Sleep time: '${sleepTime}s' " -nonewline

        Invoke-RestMethod -Method POST -uri $uri -ContentType application/json -body (Create-RequestBody $digit) | out-null
        if ($?) { Write-Host "> OK" -f green }
        Start-Sleep -s $sleepTime
    }
}
