package constructor;

import global.structure.State;
import global.tools.FileName;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;

public class Constructor {

    //Attributes
    private FileName fileName;
    private Filter filter;

    //Constructor
    public Constructor() {
        fileName = FileName.getInstance();
        filter = new Filter();
    }

    //Methods
    public void build(List<State> states) {
        List<String> output = new ArrayList<>();
        try {
            String command = "./NuSMV " + fileName.getFileName() + ".smv";
            Process process = Runtime.getRuntime().exec(command);
            BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));

            String line;
            while ((line = reader.readLine()) != null) {
                output.add(line);
            }

            process.waitFor();
        } catch (Exception e) {
            e.printStackTrace();
        }

        print(filter.process(output, states));
    }

    private void print (List<List<Step>> counterexamples) {
        String output = "";

        for (List<Step> counterexample : counterexamples) {
            int line = 1;
            output = output.concat("----------------------\n");
            for (Step step : counterexample) {
                output = output.concat(line + "\n");
                line ++;
                if (step.getState() != null)
                    output = output.concat("STATE = " + step.getState().getLabel() + "\n");
                if (step.getEvent() != null)
                    output = output.concat("EVENT = " + step.getEvent() + "\n");
                output = output.concat("DECISION = " + step.getDecision() + "\n");
            }
        }

        try {
            PrintWriter printWriter = new PrintWriter(fileName.getFileName() + ".counterexample");
            printWriter.print(output);
            printWriter.close();
            System.out.println("OK");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
