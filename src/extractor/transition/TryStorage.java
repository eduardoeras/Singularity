package extractor.transition;

import global.structure.Transition;

import java.util.ArrayList;
import java.util.List;

public class TryStorage {
    //Attributes
    private static TryStorage uniqueInstance;
    private List<Transition> response;
    private boolean isTry;
    private TransitionTools tools;

    //Constructor
    private TryStorage () {
        response = new ArrayList<>();
        isTry = false;
        tools = new TransitionTools();
    }

    //Methods
    public static TryStorage getInstance () {
        if (uniqueInstance == null) {
            uniqueInstance = new TryStorage();
        }
        return uniqueInstance;
    }

    public void store (Transition transition) {
        if (isTry) {
            response.add(transition);
        }
    }

    public List<Transition> getResponse () {
        List<Transition> clearResponse = new ArrayList<>();
        for (Transition original : response) {
            clearResponse.add(tools.createTransition(original.getEvent().getEvent(), original.getFrom(), null));
        }
        return clearResponse;
    }

    public void activeTry () {
        isTry = true;
    }

    public void deactiveTry () {
        isTry = false;
    }

    public void reset () {
        response.clear();
        isTry = false;
    }

}
