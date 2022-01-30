$tubePWM = 100
$oLedPWM = 5    # Onboard LED
$tLedPWM = 5    # Tube LEDs

$b = @{
    "tubes" = @{
        "1" = @{ "pwm" = $tubePWM }
        "2" = @{ "pwm" = $tubePWM }
        "3" = @{ "pwm" = $tubePWM }
        "4" = @{ "pwm" = $tubePWM }
    }
    "onboardLed" = @{
        "pwm" = $ledPWM
    }
    "leds" = $tLedPWM
} | convertto-json

$ProgressPreference = 'SilentlyContinue'
$targetIP = "192.168.1.176"
$uri = "http://$targetIP/api/display"

Invoke-RestMethod -Method POST -uri $uri -ContentType application/json -body $b