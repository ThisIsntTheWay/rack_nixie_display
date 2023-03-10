Param(
    [string] $NixieIP,
    [string] $MyStromIP,
    [int] $TubePWM
)

$ProgressPreference = 'SilentlyContinue'
$uri = "http://$NixieIP/api/display"

function Create-RequestBody([int[]]$ingress) {
    return @{
        "tubes" = @{
            "1" = @{ "val" = $ingress[0] }
            "2" = @{ "val" = $ingress[1] }
            "3" = @{ "val" = $ingress[2] }
            "4" = @{ "val" = $ingress[3] }
        }
    } | convertto-json
}

function Get-TubeArr([int]$integer) {
    $integer = $integer * 10

    return [int[]]@(
        [math]::Truncate(($integer / 1000) % 10),
        [math]::Truncate(($integer / 100) % 10),
        [math]::Truncate(($integer / 10) % 10),
        [math]::Truncate($integer % 10)
    )
}

# ---------------------------------------------------------------------------------------------
try {
    Write-Host "Setting onboardLed and indicators..." -f cyan
    Invoke-RestMethod -uri $uri -Method POST -contenttype application/json -body (@{
            "onboardLed" = @{ "mode" = 3}
            "indicators" = @{ "1" = $true; "2" = $false }
        } | ConvertTo-Json)
    
    # Brightness control
    $tubePwmBody = @{
        "1" = @{ "pwm" = $TubePwm }
        "2" = @{ "pwm" = $TubePwm }
        "3" = @{ "pwm" = $TubePwm }
        "4" = @{ "pwm" = $TubePwm }
    }
    
    Write-Host "Setting tube PWM..." -f cyan
    irm $uri -Method POST -ContentType application/json -body (@{
        "tubes" = $tubePwmBody
    } | ConvertTo-Json)

} catch {
    throw "Error: $_"
}

while ($true) {
    # Get power reading from myStrom
    $powerReading = [math]::Round((irm http://MyStromIp/report).power, 1)
    
    $tAsArray = Get-TubeArr $powerReading
    
    Write-Host "Power reading: " -nonewline
    Write-Host $powerReading -fore yellow -NoNewline
    
    $uri = "http://$targetIP/api/display"
    Invoke-RestMethod -Method POST -uri $uri -ContentType application/json -body (Create-RequestBody $tAsArray) | out-null
    if ($?) { Write-Host " (OK)" -f green }

    Start-Sleep -s 5
}
