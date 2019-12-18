package generator.elements;

import global.structure.Transition;

import java.util.List;

public class Initials {
    //Attributes
    private Variables variables;

    //Constructor
    public Initials () {
        variables = new Variables();
    }

    //Methods
    public String generateInitials(List<Transition> transitions) {
        String output = "";
        if (variables.existsEvent(transitions)) {
            output = "        init(events) := null;\n";
        }
        output = output.concat("        init(state) := ");
        for (Transition transition : transitions) {
            if (transition.getTo().getId() != -1) {
                output = output.concat(transition.getTo().getLabel() + "_");
                output = output.concat(transition.getTo().getId() + ";\n");
                break;
            }
        }
        return output;
    }
}
