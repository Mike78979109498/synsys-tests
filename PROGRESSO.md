# Synsys — Wi-Fi Sensing (CSI) — Progresso

Documento de orientação para retomar o trabalho em qualquer aba/sessão nova. Atualizado em 24/09/2026.

## Arquitetura decidida (fechada, não está mais em avaliação)

- Captura de CSI acontece **só nas placas ESP32 DevKitC (WROOM-32)**. Nenhuma captura no Raspberry Pi.
- Raspberry Pi 4B ("rpi-synsys") é estritamente o **"cérebro"**: recebe, processa e funde dados enviados pelas ESP32. Não sensoria.
- Nexmon foi testado, funcionou tecnicamente, mas foi **abandonado formalmente** — quebrou o Wi-Fi/DHCP do Pi (associava mas não pegava IP), motivou reformatar o Pi do zero, e a decisão foi que Broadcom+Nexmon é a ferramenta errada para esse trabalho.
- Toolchain de desenvolvimento: 100% no Pi, via terminal (SSH ou local), sem depender de Windows/VS Code. `arduino-cli` nativo (ARM64) instalado no Pi.

## Toolchain confirmado (não precisa reabrir essa investigação)

- Core instalado: `esp32:esp32@3.3.12`
- FQBN usado: `esp32:esp32:esp32` ("ESP32 Dev Module")
- CSI é suportado nativamente por esse core, sem precisar de ESP-IDF puro nem biblioteca externa (`esp-csi`):
  - Header: `~/.arduino15/packages/esp32/tools/esp32-libs/3.3.12/include/esp_wifi/include/esp_wifi.h`
  - Flag confirmada habilitada: `CONFIG_ESP_WIFI_CSI_ENABLED=y`
- Placa de referência usada nos testes: MAC `8c:94:df:4c:75:78`

## Repositório: synsys-tests (GitHub, conta Mike78979109498)

Convenção: nome da pasta = nome do sketch (exigência do `arduino-cli`).

```
synsys-tests/
├── 01_blink_placa/
├── 02_teste_serial/
│   ├── pedido_resposta/
│   └── leitura_automatica/   (script Python + CSV de teste, valores simulados)
├── 03_wifi_sensing/          (fase ativa — CSI real)
│   ├── 03_wifi_sensing.ino
│   (log_ensaio1.txt removido -- captura falhou, recaptura pendente, ver nota abaixo)
├── 04_comandos_multiplos/
├── scripts/
│   └── teste_comandos_multiplos.py
└── PROGRESSO.md              (este arquivo)
```

## Linha do tempo de validação (camada por camada)

1. **Blink** — upload de firmware básico validado nas 3 placas.
2. **Serial pedido-resposta** — ESP32 só responde a comando `LER`, validado manualmente via `screen`.
3. **Leitura automática (Python)** — script no Pi envia `LER` a cada 5s, 16 leituras dentro do range simulado, sem persistência.
4. **Persistência CSV** — 10 linhas gravadas com timestamp, validado.
5. **Múltiplos comandos** (`04_comandos_multiplos`) — protocolo de texto com PING/LED_ON/LED_OFF/STATUS e tratamento de comando desconhecido; testado via script Python interativo. Fechado como prática de reforço, não como etapa inédita do roteiro CSI.
6. **Ensaio 1 — CSI dispara** (`03_wifi_sensing`) — **validado em 24/09/2026**:
   - Conecta na rede SYNSYS_2G, habilita CSI, conta pacotes via callback.
   - `esp_wifi_set_csi_config`, `esp_wifi_set_csi_rx_cb`, `esp_wifi_set_csi(true)` — todos retornam `ESP_OK`.
   - Contador de pacotes sobe de forma consistente após boot limpo -- confirmado ao vivo, lendo a tela do `screen` (ver histórico da sessão). A tentativa de gravar esse log em arquivo (script Python + `timeout`) falhou: o `timeout` mata o processo por SIGTERM antes do `with open()` fechar/flushar, resultando em arquivo vazio. Recaptura com fechamento correto do arquivo fica pendente para a próxima sessão.
   - **Achado em aberto**: taxa de chegada de pacotes CSI não é constante — platôs de dezenas de segundos sem pacote novo, intercalados com rajadas. Hipótese mais provável: captura depende de tráfego real na rede (não gera pacotes próprios). Não decidido ainda se o sistema de presença vai precisar de tráfego artificial (ex: ping periódico) para manter cadência de amostragem estável.

## Próximo passo (Ensaio 2 — ainda não iniciado)

Extrair e visualizar o **buffer bruto de CSI** (`data->buf` dentro do callback — matriz de amplitude/fase por subportadora), um pacote por vez. Ainda sem interpretação física, só confirmar formato e faixa de valores antes de montar qualquer parser ou lógica de detecção de presença.

## Objetivo registrado para fase futura (não é a próxima etapa)

Testar a placa com alimentação independente (longe do Pi, sem USB), para validar leitura "à distância". Isso exige o sketch parar de depender da serial e passar a transmitir dados por Wi-Fi (UDP/MQTT/HTTP) para o Pi — é mudança de fase na arquitetura de telemetria, não uma alteração no ensaio atual.

## Notas de processo (para evitar repetir os mesmos tropeços)

- **Heredoc no terminal gráfico do Pi tem histórico de corromper o delimitador de fechamento** em pastes grandes (o `EOF` cola com a última linha do conteúdo, e o shell fica preso no prompt de continuação `>` em vez de executar o próximo comando). Sempre conferir com `cat` do arquivo depois de criar, e se o prompt ficar em `>`, apertar `Ctrl+C` e checar o que foi gravado antes de seguir.
- **Copiar/colar trechos de dentro do `screen`** também já produziu texto embaralhado (duas seções de log intercaladas). Prefira capturar log de boot/execução redirecionando a saída da serial direto para um arquivo via um script Python curto (`pyserial`), depois `cat` o arquivo — mais confiável que selecionar texto na tela.
- Senha da rede SYNSYS_2G já apareceu em texto puro em prints de tela durante os testes; se algum desses prints for parar em local compartilhado, considerar trocar a senha.
