# Шлюз BLE-UDP через WiFi

Шлюз на базе мини платы ESP32-C3 Super Mini (с Aliexpress)

![ESP32-C3](https://media.au.ru/imgs/400x800/92c28a5f02a39660bf11631a80d64fc7/)

В коде необходимо указать:
- Имя WiFi сети и пароль
- IP (или имя хоста) и порт UDP сервера.
Имя хоста передается в поле `gate` пакета данных.

Шлюз отправляет данные в формате JSON:

```json
{
    "gate": "esp32c3-725FAC",
    "name": "sampleBLE",
    "mac": "48:d5:60:f9:03:e1",
    "rssi": -86,
    "data": "060001092002949283710F1412B54B2CEC18F1D513E9DB24D89253AE23"
}
```

Пакет BLE для C# выглядит так:

```cs
public class BlePacket
{
    /// <summary>
    /// Имя шлюза
    /// </summary>
    [JsonPropertyName("gate")]
    public string Gate { get; set; }

    /// <summary>
    /// MAC адрес устройства
    /// </summary>
    [JsonPropertyName("mac")]
    public string Mac { get; set; }
    
    /// <summary>
    /// Имя устройства (если имеется)
    /// </summary>
    [JsonIgnore(Condition = JsonIgnoreCondition.WhenWritingNull)]
    [JsonPropertyName("name")]
    public string? Name { get; set; }

    /// <summary>
    /// Сила сигнала (RSSI)
    /// </summary>
    [JsonPropertyName("rssi")]
    public int Rssi { get; set; }

    /// <summary>
    /// Данные (если имеется)
    /// </summary>
    [JsonIgnore(Condition = JsonIgnoreCondition.WhenWritingNull)]
    [JsonPropertyName("data")]
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
