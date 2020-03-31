use std::fs;

#[test]
fn test_parse_header() -> std::io::Result<()> {
    let data = fs::read("tests/Main.class")?;
    let res = jnif::parse(&data);

    if let Ok(cf) = res {
        assert_eq!(55, cf.major);
        assert_eq!(0, cf.minor);
    }

    Ok(())
}
