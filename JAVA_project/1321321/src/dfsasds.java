
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;

public class dfsasds {
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		Socket socket = null;
		try {
			socket = new Socket();
			System.out.println("�����û:");
			socket.connect(new InetSocketAddress("127.0.0.1", 9000));
			//accept�� ����
			System.out.println("����Ϸ�:");
		}catch(Exception e) {
			e.printStackTrace();
		}
		if(!socket.isClosed()) {
			try{
				socket.close();
			
			}catch(IOException e) {
				e.getMessage();
			}
		}
		
	}

}