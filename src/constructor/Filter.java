package constructor;

import global.structure.State;

import java.util.ArrayList;
import java.util.List;

public class Filter {

    //Methods
    public List<List<Step>> process(List<String> output, List<State> states) {
        boolean flag = false;
        List<Step> counterexample = new ArrayList<>();
        List<List<Step>> counterexamples = new ArrayList<>();
        for (String line : output) {
            if (line.equals("Trace Type: Counterexample ")) {
                flag = true;
                continue;
            }
            if (flag) {
                if (line.length() >= 16 && line.substring(0,16).equals("-- specification")) {
                    flag = false;
                    if (counterexample.size() > 2) {
                        List<Step> ctrxpl = new ArrayList<>();
                        ctrxpl.addAll(counterexample);
                        counterexamples.add(ctrxpl);
                    }
                    counterexample.clear();
                    continue;
                }
                if (line.substring(0, 4).equals("  ->")) {
                    Step step = new Step();
                    counterexample.add(step);
                }
                if (line.substring(0, 7).equals("    sta")) {
                    State state = collectState(line, states);
                    counterexample.get(counterexample.size() -1).setState(state);
                }
                if (line.substring(0, 7).equals("    eve")) {
                    counterexample.get(counterexample.size() -1).setEvent(collectEvent(line));
                }
                if (line.substring(0, 7).equals("    dec")) {
                    counterexample.get(counterexample.size() -1).setDecision(collectDecision(line));
                }
            }
        }
        return counterexamples;
    }

    private boolean collectDecision(String line) {
        return line.contains("TRUE");
    }

    private String collectEvent(String line) {
        if (line.substring(13).equals("null")) return null;
        return line.substring(13);
    }

    private State collectState(String line, List<State> states) {
        int index = 0;
        for (int i = line.length() - 1; i >= 0; i--) {
            if (line.charAt(i) != '_') {
                index = i;
            } else {
                break;
            }
        }
        int id = Integer.parseInt(line.substring(index));
        for (State state : states) {
            if (state.getId() == id) {
                return state;
            }
        }
        return new State();
    }
}
