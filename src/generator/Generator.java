package generator;

import generator.elements.Initials;
import generator.elements.Nexts;
import generator.elements.Properties;
import generator.elements.Variables;
import global.structure.State;
import global.structure.Transition;
import global.tools.FileName;

import java.io.PrintWriter;
import java.util.List;

public class Generator {
    //Attributes
    private FileName fileName;
    private Variables variables;
    private Initials initials;
    private Nexts nexts;
    private Properties properties;

    //Constructor
    public Generator () {
        fileName = FileName.getInstance();
        variables = new Variables();
        initials = new Initials();
        nexts = new Nexts();
        properties = new Properties();
    }

    //Public Methods
    public List<State> generate (List<State> states, List<Transition> transitions) {
        boolean existsBoolean = variables.existsBoolean(transitions);
        String output = "";
        output = output.concat(
                "MODULE main\n" +
                "\n" +
                "    VAR\n" +
                "        state : \n" +
                "        {\n");
        output = output.concat(variables.getStates(states, transitions));
        output = output.concat(
                "        };\n");
        if (variables.existsEvent(transitions)) {
            output = output.concat("\n" +
                    "        events :\n" +
                    "        {\n");
            output = output.concat(variables.getEvents(transitions));
            output = output.concat("        };\n");
        }
        if (existsBoolean) {
            output = output.concat("\n        decision : boolean;\n");
        }
        output = output.concat(
                "\n" +
                "    ASSIGN\n" +
                "\n");
        output = output.concat(initials.generateInitials(transitions));
        output = output.concat("\n" +
                "        next(state) :=\n" +
                "            case\n");
        output = output.concat(nexts.getNextState(transitions));
        output = output.concat("                TRUE : state;\n" +
                "            esac;\n");
        if (variables.existsEvent(transitions)) {
            output = output.concat("\n" +
                    "        next(events) :=\n" +
                    "            case\n");
            output = output.concat(nexts.getNextEvents(transitions));
            output = output.concat("                TRUE : events;\n" +
                    "            esac;");
        }
        if (existsBoolean) {
            output = output.concat(
                    "\n\n" +
                    "        next(decision) :=\n" +
                    "            case\n");
            output = output.concat(nexts.getNextBoolean(transitions));
            output = output.concat("                TRUE : {TRUE, FALSE};\n" +
                    "            esac;\n");
        }
        output = output.concat("\n");
        output = output.concat(properties.generateProperties(states, transitions));
        try {
            PrintWriter printWriter = new PrintWriter(fileName.getFileName() + ".smv");
            printWriter.print(output);
            printWriter.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return states;
    }
}
