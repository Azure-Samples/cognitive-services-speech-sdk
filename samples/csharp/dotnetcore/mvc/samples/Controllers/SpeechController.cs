using Microsoft.AspNetCore.Mvc;

namespace samples.Controllers;

public sealed class SpeechController : Controller
{
    public IActionResult SpeechToText()
    {
        return View();
    }

    public IActionResult TextToSpeech()
    {
        return View();
    }
}
