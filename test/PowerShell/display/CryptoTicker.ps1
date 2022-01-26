<#
    This little script displays the current price of a specific cryptocurrency.
#>

$ProgressPreference = 'SilentlyContinue'
$targetIP = "192.168.1.176"
$uri = "http://$targetIP/api/display"

# Ticker
$asset = "BTC"
$quote = "USDT"
$binanceUri = "https://api.binance.com/api/v3/avgPrice?symbol=$asset$quote"

while ($true) {
    [int]$z = 0
    $a = [decimal](Invoke-RestMethod $binanceUri).price;
    $p = $a

    # Make sure the price is at least 3 numbers.
    if ($a -lt 1) { $a *= 1000; $z = 1 }
    elseif ($a -lt 10) { $a *= 100; $z = 2 }
    elseif ($a -lt 100) { $a *= 100; $z = 3 }
    elseif ($a -lt 1000) { $a *= 10; $z = 4 }
    elseif ($a -gt 9999) { $a /= 10; $z = 5 }
    
    $b = @{
        "tubes" = @{
            "1" = @{ "val" = [math]::Truncate($a / 1000); "pwm" = 255 }
            "2" = @{ "val" = [math]::Truncate(($a / 100) % 10); "pwm" = 255 }
            "3" = @{ "val" = [math]::Truncate(($a / 10) % 10); "pwm" = 255 }
            "4" = @{ "val" = [math]::Truncate($a % 10); "pwm" = 255 }
        }
        "indicators" = @{ "1" = $false; "2" = $false }
    }
    
    # Determine which indicator to show
    switch ($z) {
        2 { # 01.23
            $b.Indicators["2"] = $true
        }
        3 { # 12.34
            $b.Indicators["2"] = $true
        }
        4 { # 123.4
            $b.Indicators["1"] = $true
        }
    }

    Write-Host "[$(Get-Date -f 'HH:mm:ss')] " -NoNewline -fore cyan
    Write-Host "$asset/$quote -> $p " -NoNewline
    
    Invoke-RestMethod -Method POST -uri $uri -ContentType application/json -body ($b | convertto-json) | out-null
    if ($?) { Write-Host " > Update OK " -fore green }

    Start-Sleep -s 120
}