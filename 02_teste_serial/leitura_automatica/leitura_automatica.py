import serial
import time

porta = '/dev/ttyACM0'
baud = 115200

ser = serial.Serial(porta, baud, timeout=2)
time.sleep(2)  # espera a ESP32 reiniciar após abrir a porta

print("Iniciando leitura automática. Pressione Ctrl+C para parar.")

try:
    while True:
        ser.write(b'LER\n')
        resposta = ser.readline().decode('utf-8').strip()
        if resposta:
            print(f"Leitura recebida: {resposta}")
        time.sleep(5)
except KeyboardInterrupt:
    print("\nEncerrado pelo usuário.")
finally:
    ser.close()
