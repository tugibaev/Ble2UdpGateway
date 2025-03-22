# Шлюз BLE-UDP через WiFi

Шлюз на базе мини платы ESP32-C3 Super Mini (с Aliexpress)

![ESP32-C3](https://media.au.ru/imgs/400x800/92c28a5f02a39660bf11631a80d64fc7/)

В коде необходимо указать:
- Имя WiFi сети и пароль
- Имя хоста шлюза
- IP (или DNS имя) и порт UDP сервера.

Шлюз отправляет данные в формате JSON:

```json
{"Mac":"48:d5:60:f9:03:e1","Rssi":-86,"Data":"060001092002949283710F1412B54B2CEC18F1D513E9DB24D89253AE23"}
```
Без данных:

```json
{"Mac":"28:2b:b9:5b:7f:70","Rssi":-99}
```

Пакет BLE для C# выглядит так:

```cs
public class BlePacket
{
    /// <summary>
    /// MAC адрес устройства
    /// </summary>
    public string Mac { get; set; }
    
    /// <summary>
    /// Имя устройства (если имеется)
    /// </summary>
    [JsonIgnore(Condition = JsonIgnoreCondition.WhenWritingNull)]
    public string? Name { get; set; }
    
    /// <summary>
    /// Сила сигнала (RSSI)
    /// </summary>
    public int Rssi { get; set; }

    /// <summary>
    /// Данные (если имеется)
    /// </summary>
    [JsonIgnore(Condition = JsonIgnoreCondition.WhenWritingNull)]
    public string? Data { get; set; }
}
```



Микро UDP сервер для тестирования:

```cs
using System.Net.Sockets;
using System.Net;
using System.Text;

UdpClient udpServer = new UdpClient(12345);

Console.WriteLine("UDP сервер запущен и слушает порт 12345...");

try
{
    while (true)
    {
        // Принимаем данные
        IPEndPoint clientEndPoint = new IPEndPoint(IPAddress.Any, 0);
        byte[] receivedBytes = udpServer.Receive(ref clientEndPoint);
        string receivedMessage = Encoding.UTF8.GetString(receivedBytes);

        Console.WriteLine($"Получено от {clientEndPoint}: {receivedMessage}");
    }
}
catch (Exception ex)
{
    Console.WriteLine($"Ошибка: {ex.Message}");
}
finally
{
    udpServer.Close();
}
```
