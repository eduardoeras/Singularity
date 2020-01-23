package generator.elements;

import global.structure.Element;
import global.structure.Event;
import global.structure.State;
import global.structure.Transition;
import global.tools.Statistics;

import java.util.ArrayList;
import java.util.List;

public class Variables {
    //Attributes
    private Statistics statistics;

    //Constructor
    public Variables () {
        statistics = Statistics.getInstance();
    }

    //Methods
    public String getStates(List<State> states, List<Transition> transitions) {
        String output = "";
        List<State> activeStates = collectActiveStates(transitions);
        for (State state : states) {
            if (state.getId() != -1 && activeStates.contains(state)) {
                output = output.concat("            " + state.getLabel() + "_" + state.getId() + ",\n");
                statistics.addState();
                if (state.getElement().equals(Element.DECISION) || state.getElement().equals(Element.LOOP)) {
                    statistics.addDecision();
                }
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
                    statistics.addEvent();
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

    public boolean existsBoolean (List<Transition> transitions) {
        for (Transition transition : transitions) {
            if (transition.getEvent().getEvent().equals("TRUE") || transition.getEvent().getEvent().equals("FALSE")) {
                return true;
            }
        }
        return false;
    }

    public boolean existsEvent (List<Transition> transitions) {
        for (Transition transition : transitions) {
            if (!transition.getEvent().getEvent().equals("TRUE") && !transition.getEvent().getEvent().equals("FALSE") && notTrivial(transition.getEvent())) {
                return true;
            }
        }
        return false;
    }

    //Private Methods
    private List<State> collectActiveStates(List<Transition> transitions) {
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

    private boolean notTrivial(Event event) {
        return !event.getEvent().equals("TRUE") && !event.getEvent().equals("FALSE") && !event.getEvent().equals("lambda") && !event.getEvent().equals("");
    }
}
