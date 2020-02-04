package constructor;

import global.structure.Event;
import global.structure.State;
import global.structure.Transition;
import global.tools.FileName;
import global.tools.Statistics;

import java.io.*;
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

    //Public Methods
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

    //Private Methods
    private void print (List<List<Step>> counterexamples) {
        String output = "";

        int validCounterexampleCounter = 0;
        int invalidCounterexampleCounter = 0;
        boolean first = true;

        IdTransition transition = new IdTransition();

        List<IdTransition> transitions = new ArrayList<>();
        List<Integer> usedStates = new ArrayList<>();

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
                if (step.getState() != null) {
                    output = output.concat("STATE = " + step.getState().getLabel() + "\n");
                    if (transition.getTo() == null) {
                        transition.setTo(step.getState().getId());
                    } else {
                        transition.setFrom(transition.getTo());
                        transition.setTo(step.getState().getId());
                        addIfNew(transition, transitions);
                    }
                    if (!usedStates.contains(step.getState().getId())) {
                        usedStates.add(step.getState().getId());
                    }
                }
                if (step.getEvent() != null) {
                    output = output.concat("EVENT = " + step.getEvent() + "\n");
                }
                output = output.concat("DECISION = " + step.getDecision() + "\n");
            }
            transition = new IdTransition();
        }

        statistics.setTotalCounterexamples(validCounterexampleCounter + invalidCounterexampleCounter);
        statistics.setValidCounterexamples(validCounterexampleCounter);
        statistics.setInvalidCounterexamples(invalidCounterexampleCounter);
        statistics.setUsedStates(usedStates.size());
        statistics.setUsedtransitions(transitions.size());

        output = statistics.print().concat("\n" + output);

        try {
            FileWriter writer = new FileWriter("statistics.txt", true);
            writer.write(statistics.getData());
            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
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
    private void addIfNew (IdTransition transition, List<IdTransition> transitions) {
        if (!isThere(transition, transitions)) {
            transitions.add(transition.getClone());
        }
    }

    private boolean isThere (IdTransition transition, List<IdTransition> transitions) {
        for (IdTransition stored : transitions) {
            if (stored.getFrom().equals(transition.getFrom()) && stored.getTo().equals(transition.getTo())) {
                return true;
            }
        }
        return false;
    }
}
