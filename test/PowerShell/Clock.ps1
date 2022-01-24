<#
    This little script uses the display as a clock.
#>

$ProgressPreference = 'SilentlyContinue'
$targetIP = "192.168.1.176"
$uri = "http://$targetIP/api/display"

while ($true) {
    [int]$time = Get-Date -f "HHmm"

    $b = @{
        "tubes" = @{
            "1" = @{ "val" = [math]::Truncate($time / 1000); "pwm" = 255 }
            "2" = @{ "val" = [math]::Truncate(($time / 100) % 10); "pwm" = 255 }
            "3" = @{ "val" = [math]::Truncate(($time / 10) % 10); "pwm" = 255 }
            "4" = @{ "val" = [math]::Truncate($time % 10); "pwm" = 255 }
        }
        "Indicators" = @{ "1" = $false }
    } | convertto-json

    Write-Host "[$(Get-Date -f 'HH:mm:ss')] " -NoNewline -fore cyan
    Invoke-RestMethod -Method POST -uri $uri -ContentType application/json -body $b | out-null
    if ($?) { Write-Host "Update OK " -nonewline; Write-Host "($time)" -f yellow }

    Start-Sleep -s 1
}