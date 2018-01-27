
FOG_SCALE = [
    ((0, 7), "język bardzo prosty, zrozumiały już dla uczniów szkoły podstawowej"),
    ((7, 10), "język prosty, zrozumiały już dla uczniów gimnazjum"),
    ((10, 13), "język dość prosty, zrozumiały już dla uczniów liceum"),
    ((13, 16), "język dość trudny, zrozumiały dla studentów studiów licencjackich"),
    ((16, 18), "język trudny, zrozumiały dla studentów studiów magisterskich"),
    ((18, "infinity"), "język bardzo trudny, zrozumiały dla magistrów i osób z wyższym wykształceniem"),
]

PISAREK_SCALE = [
    ((0, 7.1), "tekst bardzo łatwy"),
    ((7.1, 10.1), "tekst łatwy"),
    ((10.1, 13.1), "tekst średniej trudności"),
    ((13.1, 16.1), "tekst trudny"),
    ((16.1, "infinity"), "tekst bardzo trudny"),
]

LIX_SCALE = [
    ((0, 30), "tekst bardzo łatwy"),
    ((30, 40), "tekst łatwy"),
    ((40, 50), "tekst średniej trudności"),
    ((50, 60), "tekst trudny"),
    ((60, "infinity"), "tekst bardzo trudny"),
]

GENERAL_SCALE_MESSAGES = [
    "tekst bardzo prosty",
    "tekst prosty",
    "tekst dość prosty",
    "tekst dość trudny",
    "tekst trudny",
    "tekst bardzo trudny",
]

MAX_READABILITY_POINTS = 18 + 2 * 16 + 60


def interpret_index(value, index_type):
    if value is None:
        return None
    scales_mapping = {
        'fog': FOG_SCALE,
        'pisarek': PISAREK_SCALE,
        'lix': LIX_SCALE,
    }
    scale = scales_mapping.get(index_type)
    if not scale:
        return None
    for i, val in enumerate(scale):
        scope, msg = val
        if scope[0] <= value < float(scope[1]):
            return msg, i / len(scale)
