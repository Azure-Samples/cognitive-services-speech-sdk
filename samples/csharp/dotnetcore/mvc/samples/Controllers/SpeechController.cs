using Microsoft.AspNetCore.Mvc;

namespace samples.Controllers;
public class SpeechController : Controller
{
    public IActionResult SpeechToText()
    {
        return View();
    }
}
