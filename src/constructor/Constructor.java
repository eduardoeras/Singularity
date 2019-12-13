package constructor;

import java.io.BufferedReader;
import java.io.InputStreamReader;

public class Constructor {

    //Methods
    public void build() {
        try {
            String command = "ping -c 3 www.google.com";

            Process process = Runtime.getRuntime().exec(command);

            // Read the output
            BufferedReader reader =
                    new BufferedReader(new InputStreamReader(process.getInputStream()));

            String line;
            while ((line = reader.readLine()) != null) {
                System.out.print(line + "\n");
            }

            process.waitFor();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
