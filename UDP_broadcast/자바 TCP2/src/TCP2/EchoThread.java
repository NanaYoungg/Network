package TCP2;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;

public class EchoThread extends Thread {
 public Socket sock;
 public EchoThread(Socket sock) {
	 super();
		this.sock = sock;
	 
 }
	
	public void run() {
		try {
			
			InetAddress inetaddr = sock.getInetAddress();
			System.out.println(inetaddr.getHostAddress()+"���� ����");
			OutputStream out = sock.getOutputStream();
			InputStream in = sock.getInputStream();
			
			
			PrintWriter pw = new PrintWriter(new OutputStreamWriter(out));
			BufferedReader br = new BufferedReader(new InputStreamReader(in));
			
			String line = null;
			while((line = br.readLine())!=null) {
				System.out.println("Ŭ���̾�Ʈ �Է� ���� :" + line);
				pw.println(line);
				pw.flush();
			}
			pw.close();
			br.close();
			sock.close();
			
			
		}catch(Exception e) {
			e.printStackTrace();
		}
				
		
		
	}
	
	
}
