package extractor.transition;

import global.structure.Transition;

import java.util.ArrayList;
import java.util.List;

public class ReturnControl {
    //Attributes
    private static ReturnControl uniqueInstance;
    private boolean returnFlag;
    private List<Transition> response;

    //Constructor
    private ReturnControl () {
        returnFlag = false;
        response = new ArrayList<>();
    }

    //Methods
    public static ReturnControl getInstance () {
        if (uniqueInstance == null) {
            uniqueInstance = new ReturnControl();
        }
        return uniqueInstance;
    }

    public void addResponse (List<Transition> response) {
        this.response.addAll(response);
    }

    public List<Transition> getResponse () {
        return response;
    }

    public void setReturn () {
        returnFlag = true;
    }

    public boolean getReturn () {
        return returnFlag;
    }

    public void reset () {
        returnFlag = false;
        response.clear();
    }
}
