package generator.elements;

import global.structure.Element;
import global.structure.Event;
import global.structure.Transition;

import java.util.ArrayList;
import java.util.List;

public class Nexts {

    //Methods
    public String getNextState(List<Transition> transitions) {
        String output = "";
        for (Transition transition : transitions) {
            if (transition.getFrom().getId() != -1 && transition.getTo().getId() != -1) {
                String from = transition.getFrom().getLabel() + "_" + transition.getFrom().getId();
                String to = transition.getTo().getLabel() + "_" + transition.getTo().getId();
                String event = transition.getEvent().getEvent();
                switch (transition.getFrom().getElement()) {
                    case DECISION:
                    case LOOP:
                        output = output.concat("                (state = " + from + " & decision = " + event + ") : " + to + ";\n");
                        break;
                    default:
                        output = output.concat("                (state = " + from + ") : " + to + ";\n");
                }
            }
        }
        return output;
    }

    public String getNextEvents(List<Transition> transitions) {
        String output = "";
        List<String> lines = new ArrayList<>();
        for (Transition transition : transitions) {
            if (transition.getFrom().getId() != -1 && notTrivial(transition.getEvent())) {
                switch (transition.getFrom().getElement()) {
                    case DECISION:
                    case LOOP:
                        break;
                    default:
                        String from = transition.getFrom().getLabel() + "_" + transition.getFrom().getId();
                        String event = transition.getEvent().getEvent();
                        String line = ("                (state = " + from + ") : " + event + ";\n");
                        boolean exists = false;
                        for (String l : lines) {
                            if (line.equals(l)) {
                                exists = true;
                                break;
                            }
                        }
                        if (!exists) {
                            lines.add(line);
                        }
                }
            }
        }
        for (String line : lines) {
            output = output.concat(line);
        }
        return output;
    }

    public String getNextBoolean(List<Transition> transitions) {
        String output = "";
        List<String> lines = new ArrayList<>();
        for (Transition transition : transitions) {
            String event = transition.getEvent().getEvent();
            if (event.equals("TRUE") || event.equals("FALSE")) {
                switch (transition.getFrom().getElement()) {
                    case DECISION:
                    case LOOP:
                        break;
                    default:
                        String from = transition.getFrom().getLabel() + "_" + transition.getFrom().getId();
                        String line = ("                (state = " + from + ") : " + event + ";\n");
                        boolean exists = false;
                        for (String l : lines) {
                            if (line.equals(l)) {
                                exists = true;
                                break;
                            }
                        }
                        if (!exists) {
                            lines.add(line);
                        }
                }
            }
        }
        for (String line : lines) {
            output = output.concat(line);
        }
        return output;
    }

    //Private Methods
    private boolean notTrivial(Event event) {
        return !event.getEvent().equals("TRUE") && !event.getEvent().equals("FALSE") && !event.getEvent().equals("lambda");
    }
}
