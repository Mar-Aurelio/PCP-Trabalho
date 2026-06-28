# run_benchmarks.ps1
param(
    [int]$Runs = 10
)

$Solutions = @(
    @{ Name = "random";       Exe = ".\solve_random.exe";    DrinkCount = @{ "diner"=6; "low6"=6; "high12"=3; "8-clique"=6; "12-clique"=3 } },
    @{ Name = "waiter";       Exe = ".\solve_waiter.exe";    DrinkCount = @{ "diner"=6; "low6"=6; "high12"=3; "8-clique"=6; "12-clique"=3 } },
    @{ Name = "chandy_misra"; Exe = ".\solve_CM.exe";        DrinkCount = @{ "diner"=6; "low6"=6; "high12"=3; "8-clique"=6; "12-clique"=3 } }
)

$Graphs = @(
    @{ Name = "diner";    File = ".\graphs\diner.txt"    },
    @{ Name = "low6";     File = ".\graphs\low6.txt"     },
    @{ Name = "high12";   File = ".\graphs\high12.txt"   },
    @{ Name = "8-clique"; File = ".\graphs\8-clique.txt" },
    @{ Name = "12-clique";File = ".\graphs\12-clique.txt"}
)

$ResultsDir = ".\results"

Write-Host "`n=== Compilando solucoes ===" -ForegroundColor Cyan
$CompileMap = @{
    "random"       = ".\solves\4_solve_random.cpp"
    "waiter"       = ".\solves\2_solve_waiter.cpp"
    "chandy_misra" = ".\solves\3_solve_chandy_misra.cpp"
}

foreach ($sol in $Solutions) {
    $src = $CompileMap[$sol.Name]
    Write-Host "Compilando $($sol.Name)..." -NoNewline
    $result = & g++ -std=c++17 -O2 -o $sol.Exe $src -lpthread 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Host " OK" -ForegroundColor Green
    } else {
        Write-Host " FALHOU" -ForegroundColor Red
        Write-Host $result
        exit 1
    }
}

if (-not (Test-Path $ResultsDir)) {
    New-Item -ItemType Directory -Path $ResultsDir | Out-Null
}

$TotalRuns = $Solutions.Count * $Graphs.Count * $Runs
$CurrentRun = 0

foreach ($sol in $Solutions) {
    foreach ($graph in $Graphs) {

        $outFile = "$ResultsDir\results-$($sol.Name)-$($graph.Name).txt"
        $drinks  = $sol.DrinkCount[$graph.Name]
        $currentDate = Get-Date -Format "yyyy-MM-dd HH:mm:ss"

        @(
            "================================================================",
            "  Solucao : $($sol.Name)",
            "  Grafo   : $($graph.Name) ($($graph.File))",
            "  Bebidas : $drinks por filosofo",
            "  Runs    : $Runs",
            "  Data    : $currentDate",
            "================================================================"
        ) | Out-File -FilePath $outFile -Encoding UTF8

        Write-Host "`n>>> $($sol.Name) + $($graph.Name)" -ForegroundColor Yellow

        $allTimes = @()

        for ($run = 1; $run -le $Runs; $run++) {
            $CurrentRun++
            $pct = [int](($CurrentRun / $TotalRuns) * 100)
            Write-Progress -Activity "Benchmark" `
                           -Status "$($sol.Name) / $($graph.Name) -- Run $run/$Runs" `
                           -PercentComplete $pct

            # Inicializa o processo manualmente para capturar streams sem interferencia do CLS
            $psi = New-Object System.Diagnostics.ProcessStartInfo
            $psi.FileName = $sol.Exe
            $psi.Arguments = "$($graph.File) $drinks"
            $psi.UseShellExecute = $false
            $psi.RedirectStandardOutput = $true
            $psi.RedirectStandardError = $true
            $psi.StandardOutputEncoding = [System.Text.Encoding]::UTF8

            $sw = [System.Diagnostics.Stopwatch]::StartNew()
            $proc = [System.Diagnostics.Process]::Start($psi)
            
            # Lê todas as linhas do terminal antes que o processo termine
            $outputLines = @()
            while (-not $proc.HasExited) {
                if (-not $proc.StandardOutput.EndOfStream) {
                    $line = $proc.StandardOutput.ReadLine()
                    if ($line -and $line.Trim() -ne "") {
                        $outputLines += $line
                    }
                }
            }
            # Garante a leitura do restante
            $outputLines += $proc.StandardOutput.ReadToEnd() -split "`r`n"
            $sw.Stop()

            $elapsed = [math]::Round($sw.Elapsed.TotalSeconds, 3)
            $allTimes += $elapsed

            # Pega apenas as últimas 4 linhas válidas impressas pelo executável (onde fica a tabela)
            $validLines = $outputLines | Where-Object { $_.Trim() -ne "" }
            $lastIndex = $validLines.Count - 1
            $tableOutput = ""
            if ($validLines.Count -ge 3) {
                $tableOutput = ($validLines[$lastIndex-2..$lastIndex]) -join "`r`n"
            } else {
                $tableOutput = $validLines -join "`r`n"
            }

            # Limpa sequencias de escape ANSI e caracteres de controle residuais da string final
            $tableOutput = $tableOutput -replace "\x1b\[[0-9;]*[a-zA-Z]", ""
            $tableOutput = $tableOutput -replace "\x0c", ""

            # Salva no arquivo
            @(
                "",
                "----------------------------------------------------------------",
                "  Run $run  |  Tempo total: $($elapsed)s",
                "----------------------------------------------------------------",
                $tableOutput
            ) | Out-File -FilePath $outFile -Encoding UTF8 -Append

            Write-Host "  Run $run concluida em $($elapsed)s"
        }

        # --- Estatísticas finais ---
        $avg  = [math]::Round(($allTimes | Measure-Object -Average).Average, 3)
        $min  = [math]::Round(($allTimes | Measure-Object -Minimum).Minimum, 3)
        $max  = [math]::Round(($allTimes | Measure-Object -Maximum).Maximum, 3)

        $mean     = ($allTimes | Measure-Object -Average).Average
        $variance = ($allTimes | ForEach-Object { [math]::Pow($_ - $mean, 2) } | Measure-Object -Average).Average
        $stddev   = [math]::Round([math]::Sqrt($variance), 3)
        $timesStr = $allTimes -join "s, "

        @(
            "",
            "================================================================",
            "  RESUMO ESTATISTICO -- $($sol.Name) / $($graph.Name)",
            "================================================================",
            "  Runs    : $Runs",
            "  Media   : $($avg)s",
            "  Minimo  : $($min)s",
            "  Maximo  : $($max)s",
            "  Desvio  : $($stddev)s",
            "  Tempos  : $($timesStr)s",
            "================================================================"
        ) | Out-File -FilePath $outFile -Encoding UTF8 -Append

        Write-Host "  Media: ${avg}s  Min: ${min}s  Max: ${max}s  StdDev: ${stddev}s" -ForegroundColor Cyan
    }
}