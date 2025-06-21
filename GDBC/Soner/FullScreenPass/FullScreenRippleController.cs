using UnityEngine;

public class FullScreenRippleController : MonoBehaviour
{
    public void EnableRipple()
    {
        RippleEffectToggle.Enabled = true;
    }

    public void DisableRipple()
    {
        RippleEffectToggle.Enabled = false;
    }

    public void ToggleRipple()
    {
        RippleEffectToggle.Enabled = !RippleEffectToggle.Enabled;
    }

    void Update()
    {
        if (Input.GetMouseButtonDown(0)) {
            RippleEffectToggle.Enabled = !RippleEffectToggle.Enabled;
            Debug.Log("Ripple Effect Enabled: " + RippleEffectToggle.Enabled);
        }
    }
}
