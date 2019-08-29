package extractor.gears;

import global.structure.State;
import global.structure.Element;
import global.structure.Type;
import global.structure.Visibility;

import java.util.List;

public class StatePrinter {
    //Attrbutes

    //Constructor

    //Methods
    public void print (List<State> states) {
        for (State state : states) {
            String output = "";
            for (int i = 0; i < state.getScopeLevel(); i++) {
                System.out.print("    ");
            }

            if (state.getType() == Type.STATE) {
                output = "<state label='";
            } else {
                if (state.getElement() == Element.NONE) {
                    System.out.println("</level>");
                    continue;
                } else {
                    output = "<level label='";
                }
            }

            output = output.concat(state.getLabel());
            output = output.concat("' element='");
            output = output.concat(state.getElement().toString());

            if (state.getVisibility() != Visibility.NONE) {
                output = output.concat("' visibility='");
                output = output.concat(state.getVisibility().toString());
            }

            if (state.getId() != -1) {
                output = output.concat("' id='");
                output = output.concat(String.valueOf(state.getId()));
            }

            output = output.concat("'>");
            System.out.println(output);
        }
    }
}
