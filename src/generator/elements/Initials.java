package generator.elements;

import global.structure.Transition;

import java.util.List;

public class Initials {

    //Methods
    public String generateInitials(List<Transition> transitions) {
        String output = "        init(events) := null;\n";
        output = output.concat("        init(state) := ");
        output = output.concat(transitions.get(0).getTo().getLabel() + "_");
        output = output.concat(transitions.get(0).getTo().getId() + ";\n");
        return output;
    }
}
