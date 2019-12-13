package generator.elements;

import global.structure.Event;
import global.structure.State;
import global.structure.Transition;

import java.util.List;

public class Properties {

    //Methods
    public String generateProperties(List<State> states, List<Transition> transitions) {
        String output = "";
        output = output.concat(caseOne(transitions));
        output = output.concat("\n");
        output = output.concat(caseTwo(transitions));
        output = output.concat("\n");
        output = output.concat(caseThree(transitions));
        return output;
    }

    //Private Methods
    private String caseOne(List<Transition> transitions) {
        String output = "";
        for (Transition transition : transitions) {
            if (notTrivial(transition.getEvent())) {
                output = output.concat("CTLSPEC\n   AG (events != " + transition.getEvent().getEvent() + ")\n");
            }
        }
        return output;
    }

    private String caseTwo(List<Transition> transitions) {
        String output = "";
        for (Transition transition : transitions) {
            if (transition.getFrom().getId() != -1 && transition.getTo().getId() != -1) {
                String from = transition.getFrom().getLabel() + "_" + transition.getFrom().getId();
                String to = transition.getTo().getLabel() + "_" + transition.getTo().getId();
                String event = transition.getEvent().getEvent();
                switch (transition.getFrom().getElement()) {
                    case DECISION:
                    case LOOP:
                        output = output.concat("CTLSPEC\n    AG (state = " + from + " & decision = " + event + " -> EX state != " + to + ")\n");
                        break;
                }
            }
        }
        return output;
    }

    private String caseThree(List<Transition> transitions) {
        String output = "";
        for (Transition transition : transitions) {
            if (transition.getFrom().getId() != -1 && transition.getTo().getId() != -1) {
                String from = transition.getFrom().getLabel() + "_" + transition.getFrom().getId();
                String to = transition.getTo().getLabel() + "_" + transition.getTo().getId();
                output = output.concat("CTLSPEC\n    AG (state = " + from + " -> EX state != " + to + ")\nCTLSPEC\n   AG (state != " + from + " -> EX state = " + to + ")\n");
            }
        }
        return output;
    }

    private boolean notTrivial(Event event) {
        return !event.getEvent().equals("TRUE") && !event.getEvent().equals("FALSE") && !event.getEvent().equals("lambda");
    }
}
