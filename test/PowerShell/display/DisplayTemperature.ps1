<#
    This little script shows the current temperature as recorded by the TMP36 sensor.
    The raw ADC reading is used as base for calculations.
#>

$ProgressPreference = 'SilentlyContinue'
$targetIP = "192.168.1.176"
$uri = "http://$targetIP/api/display"

function Create-RequestBody([int[]]$ingress) {
    return @{
        "tubes" = @{
            "1" = @{ "val" = $ingress[0]; "pwm" = 255 }
            "2" = @{ "val" = $ingress[1]; "pwm" = 255 }
            "3" = @{ "val" = $ingress[2]; "pwm" = 255 }
            "4" = @{ "val" = $ingress[3]; "pwm" = 255 }
        }
    } | convertto-json
}

function Calculate-Temperature([int]$raw, [bool]$asArray = $true) {
    $v = $raw / 1023.0
    $t = [Math]::truncate(($v - 0.5) * 10000)

    if ($asArray) {
        return [int[]]@(
            [math]::Truncate($t / 1000),
            [math]::Truncate(($t / 100) % 10),
            [math]::Truncate(($t / 10) % 10),
            [math]::Truncate($t % 10)
        )
    } else {
        return $t / 100
    }
}

# ---------------------------------------------------------------------------------------------
Invoke-RestMethod -uri $uri -Method POST -contenttype application/json -body (@{
        "onboardLed" = @{ "mode" = 3}
        "Indicators" = @{ "1" = $false; "2" = $true }
    } | ConvertTo-Json)

while ($true) {
    $uri = "http://$targetIP/api/temperature"
    $systemTemperature = (Invoke-RestMethod $uri).temperatureRaw
    
    $tAsNumber = Calculate-Temperature $systemTemperature $false
    $tAsArray = Calculate-Temperature $systemTemperature
    
    Write-Host "Temperature (°C): " -nonewline
    Write-Host $tAsNumber -fore yellow -NoNewline
    
    $uri = "http://$targetIP/api/display"
    Invoke-RestMethod -Method POST -uri $uri -ContentType application/json -body (Create-RequestBody $tAsArray) | out-null
    if ($?) { Write-Host " (OK)" -f green }
}
