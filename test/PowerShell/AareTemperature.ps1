<#
    This little script displays the current Aare temperature in bern.
#>

$ProgressPreference = 'SilentlyContinue'
$targetIP = "192.168.1.176"
$aareUri = "https://aareguru.existenz.ch/v2018/current?app=app.thisisnttheway.rack-nixie-display&version=1?values=aare.temperature"
$uri = "http://$targetIP/api/display"

while ($true) {
    $a = (Invoke-RestMethod $aareUri).aare.temperature_prec * 100
    
    $b = @{
        "tubes" = @{
            "1" = @{ "val" = [math]::Truncate($a / 1000); "pwm" = 255 }
            "2" = @{ "val" = [math]::Truncate(($a / 100) % 10); "pwm" = 255 }
            "3" = @{ "val" = [math]::Truncate(($a / 10) % 10); "pwm" = 255 }
            "4" = @{ "val" = [math]::Truncate($a % 10); "pwm" = 255 }
        }
        "Indicators" = @{ "1" = $false }
    } | convertto-json

    Write-Host "[$(Get-Date -f 'HH:mm:ss')] " -NoNewline -fore cyan
    Write-Host "$($a / 100)°C " -NoNewline
    
    Invoke-RestMethod -Method POST -uri $uri -ContentType application/json -body $b | out-null
    if ($?) { Write-Host " > Update OK " -fore green }

    Start-Sleep -s 120
}