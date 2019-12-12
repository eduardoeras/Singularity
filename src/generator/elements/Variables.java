package generator.elements;

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
        return output;
    }

    public String getEvents(List<Transition> transitions) {
        String output = "";
        for (Transition transition : transitions) {
            if(!transition.getEvent().getEvent().equals("TRUE") &&
                    !transition.getEvent().getEvent().equals("FALSE") &&
                    !transition.getEvent().getEvent().equals("lambda")) {
                output = output.concat("            " + transition.getEvent().getEvent() + ",\n");
            }
        }
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
