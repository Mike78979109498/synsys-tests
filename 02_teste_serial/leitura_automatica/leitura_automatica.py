import serial
import time
import csv
from datetime import datetime

porta = '/dev/ttyACM0'
baud = 115200
arquivo_csv = 'leituras.csv'

ser = serial.Serial(porta, baud, timeout=2)
time.sleep(2)

# Cria o arquivo com cabeçalho, se ainda não existir
try:
    with open(arquivo_csv, 'x', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['timestamp', 'valor'])
except FileExistsError:
    pass

print(f"Iniciando leitura automática. Gravando em {arquivo_csv}. Ctrl+C para parar.")

try:
    while True:
        ser.write(b'LER\n')
        resposta = ser.readline().decode('utf-8').strip()
        if resposta:
            agora = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            print(f"{agora} — Leitura recebida: {resposta}")
            with open(arquivo_csv, 'a', newline='') as f:
                writer = csv.writer(f)
                writer.writerow([agora, resposta])
        time.sleep(5)
except KeyboardInterrupt:
    print("\nEncerrado pelo usuário.")
finally:
    ser.close()
