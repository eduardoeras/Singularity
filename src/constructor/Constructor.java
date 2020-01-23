package constructor;

import global.structure.State;
import global.tools.FileName;
import global.tools.Statistics;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;

public class Constructor {

    //Attributes
    private FileName fileName;
    private Filter filter;
    private Statistics statistics;

    //Constructor
    public Constructor() {
        fileName = FileName.getInstance();
        filter = new Filter();
        statistics = Statistics.getInstance();
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

        int validCounterexampleCounter = 0;
        int invalidCounterexampleCounter = 0;
        boolean first = true;

        for (List<Step> counterexample : counterexamples) {
            if (counterexample.size() < 3) {
                invalidCounterexampleCounter ++;
                continue;
            }
            if (first) {
                statistics.setSmallestCounterexample(counterexample.size());
                first = false;
            }
            if (counterexample.size() > statistics.getBiggestCounterexample()) {
                statistics.setBiggestCounterexample(counterexample.size());
            }
            if (counterexample.size() < statistics.getSmallestCounterexample()) {
                statistics.setSmallestCounterexample(counterexample.size());
            }
            int line = 1;
            validCounterexampleCounter ++;
            output = output.concat("---------------------- " + validCounterexampleCounter + "\n");
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

        statistics.setTotalCounterexamples(validCounterexampleCounter + invalidCounterexampleCounter);
        statistics.setValidCounterexamples(validCounterexampleCounter);
        statistics.setInvalidCounterexamples(invalidCounterexampleCounter);

        output = statistics.print().concat("\n" + output);

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
