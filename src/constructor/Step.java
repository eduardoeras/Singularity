package constructor;

import global.structure.State;

public class Step {
    //Attributes
    private State state;
    private String event;
    private boolean decision;

    //Constructor

    //Methods
    public State getState() {
        return state;
    }

    public void setState(State state) {
        this.state = state;
    }

    public String getEvent() {
        return event;
    }

    public void setEvent(String event) {
        this.event = event;
    }

    public boolean getDecision() {
        return decision;
    }

    public void setDecision(boolean decision) {
        this.decision = decision;
    }

}
