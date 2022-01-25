<#
    This little script uses the display as a clock.
#>

$ProgressPreference = 'SilentlyContinue'
$targetIP = "192.168.1.176"
$uri = "http://$targetIP/api/display"

while ($true) {
    [int]$time = Get-Date -f "HHmm"

    # Invert indicator
    $indicator = [bool]!((Invoke-RestMethod ($uri + "/indicators")).indicators.2)

    $b = @{
        "tubes" = @{
            "1" = @{ "val" = [math]::Truncate($time / 1000); "pwm" = 255 }
            "2" = @{ "val" = [math]::Truncate(($time / 100) % 10); "pwm" = 255 }
            "3" = @{ "val" = [math]::Truncate(($time / 10) % 10); "pwm" = 255 }
            "4" = @{ "val" = [math]::Truncate($time % 10); "pwm" = 255 }
        }
        "indicators" = @{
            "1" = $false
            "2" = $indicator
        }
    } | ConvertTo-Json
    Write-Host "[$(Get-Date -f 'HH:mm:ss')] " -NoNewline -fore cyan
    
    $stopwatch =  [system.diagnostics.stopwatch]::StartNew()
    Invoke-RestMethod -Method POST -uri $uri -ContentType application/json -body $b | out-null
    if ($?) { $stopwatch.Stop(); Write-Host "Update OK " -nonewline -f green; Write-Host "($($stopwatch.ElapsedMilliseconds)ms)" -f yellow }

    Start-Sleep -s 1
}