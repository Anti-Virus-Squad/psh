using System;
using System.IO.Ports;
using System.Threading;

public class Port
{
    static bool flag;   //设置一个变量用于判断输出与接收操作
    static SerialPort NewPort;  //设置一个新的串口

    public static void Main()
    {
        string name;
        string message;
        StringComparer stringComparer = StringComparer.OrdinalIgnoreCase;
        Thread readThread = new Thread(Read);
        // 创建一个新的串行接口
        NewPort = new SerialPort();
        // 设置读写超时参数
        NewPort.ReadTimeout = 500;
        NewPort.WriteTimeout = 500;
        // 用户对串口的参数进行设定
        NewPort.PortName = SetPortName();
        NewPort.BaudRate = SetPortBaudRate();
        NewPort.Parity = SetPortParity(NewPort.Parity);
        NewPort.DataBits = SetPortDataBits();
        NewPort.StopBits = SetPortStopBits(NewPort.StopBits);
        NewPort.Handshake = SetPortHandshake(NewPort.Handshake);
        //串口参数设置完毕，打开此串口
        NewPort.Open();
        flag = true;
        readThread.Start();
        Console.Write("输入用户名: ");
        name = Console.ReadLine();
        Console.WriteLine("请输入您要发送的信息：（输入“QUIT”以退出）");
        while (flag)
        {
            message = Console.ReadLine();

            if (stringComparer.Equals("quit", message))
            {
                flag = false;
            }
            else
            {
                //端口发出对应信息
                NewPort.WriteLine(String.Format("<{0}>: {1}", name, message));
                //发送信息在发送端显示出相应结果
                //获取系统时间
                System.DateTime sendTime = new System.DateTime();
                sendTime = System.DateTime.Now;
                Console.WriteLine("[发送  " + sendTime + "." + sendTime.Millisecond + "]  " + String.Format("<{0}>: {1}", name, message));
            }
        }
        readThread.Join();
        NewPort.Close();
    }

    //接收端接收数据
    public static void Read()
    {
        while (flag)
        {
            //接收信息并在接收端显示出相应结果
            try
            {
                //获取系统时间
                string message = NewPort.ReadLine();
                System.DateTime reciveTime = new System.DateTime();
                reciveTime = System.DateTime.Now;
                Console.WriteLine("[接收  " + reciveTime + "." + reciveTime.Millisecond + "]  " + message);
            }
            catch (TimeoutException) { }
        }
    }

    //输入要连接的端口名称
    public static string SetPortName()
    {
        string portName;
        Console.WriteLine("请设置要连接的COM端口，可用的的端口有:");
        foreach (string s in SerialPort.GetPortNames())
        {
            Console.WriteLine("   {0}", s);
        }
        Console.Write("请输入COM端口 : ");
        portName = Console.ReadLine();
        while(portName == "" || !(portName.ToLower()).StartsWith("com"))
        {
            Console.WriteLine("输入的COM端口不符合规则，请重新输入！");
            portName = Console.ReadLine();
        }
        return portName;
    }

    // 设置串口传输的波特率
    public static int SetPortBaudRate()
    {
        string baudRate;
        int defaultBaudrate = 4800;
        Console.Write("请输入要设置的波特率(默认值:{0}): ", defaultBaudrate);
        baudRate = Console.ReadLine();

        if (baudRate == "")
        {
            baudRate = defaultBaudrate.ToString();
        }

        return int.Parse(baudRate);
    }

    // 设置端口奇偶校验值
    public static Parity SetPortParity(Parity defaultPortParity)
    {
        string parity;

        Console.Write("请输入要设置的奇偶校验值 Odd/Even (默认: {0}):", defaultPortParity.ToString(), true);
        parity = Console.ReadLine();
        if (parity == "")
        {
            parity = defaultPortParity.ToString();
        }
        return (Parity)Enum.Parse(typeof(Parity), parity, true);
    }

    // 设置串口的数据位
    public static int SetPortDataBits()
    {
        string dataBits;
        int defaultDatabit = 8;
        Console.Write("输入要设置的数据位 (默认: {0}): ", defaultDatabit);
        dataBits = Console.ReadLine();

        if (dataBits == "")
        {
            dataBits = defaultDatabit.ToString();
        }
        return int.Parse(dataBits.ToUpperInvariant());
    }

    // 设置串口的停止位
    public static StopBits SetPortStopBits(StopBits defaultPortStopBits)
    {
        string stopBits;

        Console.Write("请输入要设置的停止位 One/Two/OnePointFive (默认: {0}):", defaultPortStopBits.ToString());
        stopBits = Console.ReadLine();

        if (stopBits == "")
        {
            stopBits = defaultPortStopBits.ToString();
        }

        return (StopBits)Enum.Parse(typeof(StopBits), stopBits, true);
    }

    //设置握手协议值
    public static Handshake SetPortHandshake(Handshake defaultPortHandshake)
    {
        string handshake;

        Console.WriteLine("设置握手协议值,可用的选择有:");
        foreach (string s in Enum.GetNames(typeof(Handshake)))
        {
            Console.WriteLine("   {0}", s);
        }

        Console.Write("请输入握手协议值 (默认: {0}):", defaultPortHandshake.ToString());
        handshake = Console.ReadLine();

        if (handshake == "")
        {
            handshake = defaultPortHandshake.ToString();
        }

        return (Handshake)Enum.Parse(typeof(Handshake), handshake, true);
    }
}
