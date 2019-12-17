package constructor;

import global.structure.State;
import global.tools.FileName;

import java.io.BufferedReader;
import java.io.InputStreamReader;
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
        for (List<Step> counterexample : counterexamples) {
            int line = 1;
            System.out.println("----------------------");
            for (Step step : counterexample) {
                System.out.println(line);
                line ++;
                if (step.getState() != null)
                    System.out.println("STATE = " + step.getState().getLabel());
                if (step.getEvent() != null)
                    System.out.println("EVENT = " + step.getEvent());
                System.out.println("DECISION = " + step.getDecision());
            }
        }
    }
}
