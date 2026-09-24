#!/usr/bin/env python3
import sys
import time

import serial

PORT_PADRAO = "/dev/ttyACM0"
BAUD = 115200


def main():
    porta = sys.argv[1] if len(sys.argv) > 1 else PORT_PADRAO

    print(f"Conectando em {porta} @ {BAUD}...")
    ser = serial.Serial(porta, BAUD, timeout=2)

    time.sleep(2)
    ser.reset_input_buffer()

    print("Conectado. Comandos: PING, LED_ON, LED_OFF, STATUS. Digite 'sair' pra encerrar.\n")

    try:
        while True:
            cmd = input("> ").strip()
            if cmd.lower() in ("sair", "exit", "quit"):
                break
            if not cmd:
                continue

            ser.write((cmd + "\n").encode("utf-8"))

            time.sleep(0.15)
            respondeu = False
            while ser.in_waiting:
                linha = ser.readline().decode("utf-8", errors="replace").strip()
                if linha:
                    print(f"ESP32: {linha}")
                    respondeu = True

            if not respondeu:
                print("(sem resposta -- confira se a placa esta ligada e no sketch certo)")

    except KeyboardInterrupt:
        print("\nInterrompido pelo usuario.")
    finally:
        ser.close()
        print("Porta serial fechada.")


if __name__ == "__main__":
    main()
