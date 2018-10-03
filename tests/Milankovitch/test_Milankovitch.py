from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_Milankovitch():
    """Test modules distorb, distrot, and poise."""
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Run our comparisons
    assert np.isclose(output.log.final.Earth.TGlobal, 14.649776)
    assert np.isclose(output.log.final.Earth.Eccentricity, 0.0167)
    assert np.isclose(output.log.final.Earth.Obliquity, 0.4091)
    assert np.isclose(output.log.final.Earth.AlbedoGlobal, 0.344298)
    assert np.isclose(output.log.final.Earth.FluxOutGlobal, 233.918031)

    assert np.isclose(output.log.final.Mercury.Eccentricity, 0.205631)
    assert np.isclose(output.log.final.Saturn.LongA, 1.984656)
    assert np.isclose(output.log.final.George.ArgP, 1.688334)


if __name__ == "__main__":
    test_Milankovitch()