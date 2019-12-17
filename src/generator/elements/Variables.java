package generator.elements;

import global.structure.Event;
import global.structure.State;
import global.structure.Transition;

import java.util.ArrayList;
import java.util.List;

public class Variables {

    //Methods
    public String getStates(List<State> states, List<Transition> transitions) {
        String output = "";
        List<State> activeStates = colectActiveStates(transitions);
        for (State state : states) {
            if (state.getId() != -1 && activeStates.contains(state)) {
                output = output.concat("            " + state.getLabel() + "_" + state.getId() + ",\n");
            }
        }
        output = output.substring(0, output.length() - 2) + "\n";
        return output;
    }

    public String getEvents(List<Transition> transitions) {
        List<String> events = new ArrayList<>();
        String output = "            null,\n";
        boolean valid = false;
        for (Transition transition : transitions) {
            if(!transition.getEvent().getEvent().equals("TRUE") &&
                    !transition.getEvent().getEvent().equals("FALSE") &&
                    !transition.getEvent().getEvent().equals("lambda")) {
                if (!events.contains(transition.getEvent().getEvent()) && !transition.getEvent().getEvent().equals("")) {
                    events.add(transition.getEvent().getEvent());
                    valid = true;
                    output = output.concat("            " + transition.getEvent().getEvent());
                }
            }
            if (valid) {
                output = output.concat(",\n");
                valid = false;
            }
        }
        output = output.substring(0, output.length() - 2) + "\n";
        return output;
    }

    public boolean existitsBoolean(List<Transition> transitions) {
        for (Transition transition : transitions) {
            if (transition.getEvent().getEvent().equals("TRUE") || transition.getEvent().getEvent().equals("FALSE")) {
                return true;
            }
        }
        return false;
    }

    //Private Methods
    private List<State> colectActiveStates(List<Transition> transitions) {
        List<State> activeStates = new ArrayList<>();
        for (Transition transition : transitions) {
            if (!activeStates.contains(transition.getFrom())) {
                activeStates.add(transition.getFrom());
            }
            if (!activeStates.contains(transition.getTo())) {
                activeStates.add(transition.getTo());
            }
        }
        return activeStates;
    }
}
