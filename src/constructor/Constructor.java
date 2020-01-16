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
        String header = "";

        int validCounterexampleCounter = 0;
        int invalidCounterexampleCounter = 0;
        int bigger = 0;
        for (List<Step> counterexample : counterexamples) {
            if (counterexample.size() < 3) {
                invalidCounterexampleCounter ++;
                continue;
            }
            if (counterexample.size() > bigger) {
                bigger = counterexample.size();
            }
            int line = 1;
            validCounterexampleCounter ++;
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

        statistics.setTotalCounterexamples(validCounterexampleCounter + invalidCounterexampleCounter);
        statistics.setValidCounterexamples(validCounterexampleCounter);
        statistics.setInvalidCounterexamples(invalidCounterexampleCounter);

        header = header.concat("Name of the input C++ file: " + fileName.getFileName() + "\n");
        header = header.concat("Number of states: " + statistics.getStates() + "\n");
        header = header.concat("Number of events: " + statistics.getEvents() + "\n");
        if (statistics.getDecision()) {
            header = header.concat("Is there decisions? Yes\n");
        } else {
            header = header.concat("Is there decisions? No\n");
        }
        header = header.concat("Number of transitions: " + statistics.getTransitions() + "\n");
        header = header.concat("..................................\n");
        header = header.concat("Number of Case One properties: " + statistics.getPropertiesCaseOne() + "\n");
        header = header.concat("Number of Case Two properties: " + statistics.getPropertiesCaseTwo() + "\n");
        header = header.concat("Number of Case Three properties: " + statistics.getPropertiesCaseThree() + "\n");
        header = header.concat("Total Number of properties: " + statistics.getProperties() + "\n");
        header = header.concat("..................................\n");
        header = header.concat("Number of Counterexamples: " + statistics.getTotalCounterexamples() + "\n");
        header = header.concat("Number of Valid Counterexamples: " + statistics.getValidCounterexamples() + "\n");
        header = header.concat("Number of Invalid Counterexamples: " + statistics.getInvalidCounterexamples() + "\n");
        header = header.concat("Bigger number of states in a counterexample: " + bigger + "\n");

        System.out.println(header);

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
